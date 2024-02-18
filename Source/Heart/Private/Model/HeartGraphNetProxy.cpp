﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNetProxy.h"

#include "Model/HeartGraphNode.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNetProxy)

DEFINE_LOG_CATEGORY(LogHeartNet)

void FHeartReplicatedNodeData::PostReplicatedAdd(const FHeartReplicatedGraphNodes& Array)
{
	Array.OwningProxy->UpdateNodeProxy(*this);
}

void FHeartReplicatedNodeData::PostReplicatedChange(const FHeartReplicatedGraphNodes& Array)
{
	Array.OwningProxy->UpdateNodeProxy(*this);
}

void FHeartReplicatedNodeData::PreReplicatedRemove(const FHeartReplicatedGraphNodes& Array)
{
	Array.OwningProxy->RemoveNodeProxy(NodeGuid);
}

void FHeartReplicatedNodeData::PostReplicatedReceive(
	const FFastArraySerializer::FPostReplicatedReceiveParameters& Parameters)
{
}

FString FHeartReplicatedNodeData::GetDebugString()
{
	return NodeGuid.ToString();
}

int32 FHeartReplicatedGraphNodes::IndexOf(const FHeartNodeGuid& Guid) const
{
	// @todo replace with faster search than linear:
	// keep nodes sorted somehow and use binary search probably?
	return Items.IndexOfByPredicate(
		[Guid](const FHeartReplicatedNodeData& NodeData)
		{
			return NodeData.NodeGuid == Guid;
		});
}

void FHeartReplicatedGraphNodes::Operate(const FHeartNodeGuid& Guid, const TFunctionRef<void(FHeartReplicatedNodeData&)>& Func)
{
	const int32 Index = IndexOf(Guid);

	if (Index != INDEX_NONE)
	{
		FHeartReplicatedNodeData& Item = Items[Index];
		Func(Item);
		MarkItemDirty(Item);
	}
	else
	{
		FHeartReplicatedNodeData& Item = Items.AddDefaulted_GetRef();
		Item.NodeGuid = Guid;
		Func(Item);
		MarkItemDirty(Item);
	}
}

bool FHeartReplicatedGraphNodes::Delete(const FHeartNodeGuid& Guid)
{
	const int32 Index = IndexOf(Guid);

	if (Index != INDEX_NONE)
	{
		Items.RemoveAt(Index);
		MarkArrayDirty();
		return true;
	}

	return false;
}

UHeartGraphNetProxy::UHeartGraphNetProxy()
{
	ReplicatedNodes.OwningProxy = this;
}

UWorld* UHeartGraphNetProxy::GetWorld() const
{
	if (const AActor* MyOwner = GetOwningActor())
	{
		return MyOwner->GetWorld();
	}
	return nullptr;
}

void UHeartGraphNetProxy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ReplicatedNodes);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	//SharedParams.Condition = COND_InitialOnly; // @todo enable this once it works
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, GraphClass, Params);

	// Replicate any properties added to BP children of this class
	if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}
}

bool UHeartGraphNetProxy::IsSupportedForNetworking() const
{
	return true;
}

int32 UHeartGraphNetProxy::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if (AActor* Actor = GetOwningActor())
	{
		return Actor->GetFunctionCallspace(Function, Stack);
	}
	return Super::GetFunctionCallspace(Function, Stack);
}

bool UHeartGraphNetProxy::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));
	AActor* Owner = GetOwningActor();
	if (UNetDriver* NetDriver = Owner->GetNetDriver())
	{
		NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
		return true;
	}
	return false;
}

AActor* UHeartGraphNetProxy::GetOwningActor() const
{
	if (!IsTemplate())
	{
		return GetOuterAActor();
	}
	return nullptr;
}

UHeartGraph* UHeartGraphNetProxy::GetGraph() const
{
	if (GetOwningActor()->HasAuthority())
	{
		return SourceGraph;
	}
	return ProxyGraph;
}

UHeartGraphNetProxy* UHeartGraphNetProxy::CreateHeartNetProxy(AActor* Owner, UHeartGraph* SourceGraph)
{
	ensure(Owner->GetIsReplicated());
	ensure(Owner->IsUsingRegisteredSubObjectList());

	UHeartGraphNetProxy* NewProxy = NewObject<UHeartGraphNetProxy>(Owner);

	Owner->AddReplicatedSubObject(NewProxy);
	ensure(Owner->IsReplicatedSubObjectRegistered(NewProxy));

	NewProxy->SetupGraphProxy(SourceGraph);

	return NewProxy;
}

void UHeartGraphNetProxy::Destroy()
{
	if (IsValid(this))
	{
		checkf(GetOwningActor()->HasAuthority() == true, TEXT("Destroy:: Object does not have authority to destroy itself!"));

		OnDestroyed();
		MarkAsGarbage();
	}
}

void UHeartGraphNetProxy::RequestUpdateNode(UHeartGraphNode* Node)
{
	UpdateReplicatedNodeData(Node);
}

bool UHeartGraphNetProxy::SetupGraphProxy(UHeartGraph* InSourceGraph)
{
	if (!IsValid(InSourceGraph))
	{
		return false;
	}

	SourceGraph = InSourceGraph;

	SourceGraph->GetOnNodeAdded().AddUObject(this, &ThisClass::OnNodeAdded_Source);
	SourceGraph->GetOnNodeMoved().AddUObject(this, &ThisClass::OnNodesMoved_Source);
	SourceGraph->GetOnNodeRemoved().AddUObject(this, &ThisClass::OnNodeRemoved_Source);
	SourceGraph->GetOnNodeConnectionsChanged().AddUObject(this, &ThisClass::OnNodeConnectionsChanged_Source);

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, GraphClass, this);
	GraphClass = SourceGraph->GetClass();

	SourceGraph->ForEachNode(
		[this](UHeartGraphNode* Node)
		{
			if (ShouldReplicateNode(Node))
			{
				UpdateReplicatedNodeData(Node);
			}

			// Continue iterating
			return true;
		});

	return true;
}

void UHeartGraphNetProxy::OnNodeAdded_Source(UHeartGraphNode* HeartGraphNode)
{
	UpdateReplicatedNodeData(HeartGraphNode);
}

void UHeartGraphNetProxy::OnNodesMoved_Source(const FHeartNodeMoveEvent& NodeMoveEvent)
{
	if (NodeMoveEvent.MoveFinished)
	{
		for (auto Element : NodeMoveEvent.AffectedNodes)
		{
			UpdateReplicatedNodeData(Element);
		}
	}
}

void UHeartGraphNetProxy::OnNodeRemoved_Source(UHeartGraphNode* HeartGraphNode)
{
	RemoveReplicatedNodeData(HeartGraphNode->GetGuid());
}

void UHeartGraphNetProxy::OnNodeConnectionsChanged_Source(const FHeartGraphConnectionEvent& GraphConnectionEvent)
{
	for (auto Element : GraphConnectionEvent.AffectedNodes)
	{
		UpdateReplicatedNodeData(Element);
	}
}

bool UHeartGraphNetProxy::ShouldReplicateNode(UHeartGraphNode* Node) const
{
	return true;
}

void UHeartGraphNetProxy::UpdateReplicatedNodeData(UHeartGraphNode* Node)
{
	if (!IsValid(Node)) return;

	ReplicatedNodes.Operate(Node->GetGuid(),
		[Node](FHeartReplicatedNodeData& NodeData)
		{
			NodeData.FlakeData = Heart::Flakes::CreateFlake(Node);
			UE_LOG(LogHeartNet, Log, TEXT("Updated replicated node '%s' (%i bytes)"),
				*Node->GetName(), NodeData.FlakeData.Data.Num());
		});
}

void UHeartGraphNetProxy::RemoveReplicatedNodeData(const FHeartNodeGuid& Node)
{
	ReplicatedNodes.Delete(Node);
}

UHeartGraph* UHeartGraphNetProxy::GetProxiedGraph() const
{
	return ProxyGraph;
}

void UHeartGraphNetProxy::OnRep_GraphClass()
{
	ensure(!ProxyGraph && GraphClass);

	ProxyGraph = NewObject<UHeartGraph>(this, GraphClass);

	ProxyGraph->GetOnNodeAdded().AddUObject(this, &ThisClass::OnNodeAdded_Proxy);
	ProxyGraph->GetOnNodeMoved().AddUObject(this, &ThisClass::OnNodesMoved_Proxy);
	ProxyGraph->GetOnNodeRemoved().AddUObject(this, &ThisClass::OnNodeRemoved_Proxy);
	ProxyGraph->GetOnNodeConnectionsChanged().AddUObject(this, &ThisClass::OnNodeConnectionsChanged_Proxy);

	for (auto&& Element : ReplicatedNodes.Items)
	{
		UpdateNodeProxy(Element);
	}
}

void UHeartGraphNetProxy::OnNodeAdded_Proxy(UHeartGraphNode* HeartGraphNode)
{
}

void UHeartGraphNetProxy::OnNodesMoved_Proxy(const FHeartNodeMoveEvent& NodeMoveEvent)
{
}

void UHeartGraphNetProxy::OnNodeRemoved_Proxy(UHeartGraphNode* HeartGraphNode)
{
}

void UHeartGraphNetProxy::OnNodeConnectionsChanged_Proxy(const FHeartGraphConnectionEvent& GraphConnectionEvent)
{
}

bool UHeartGraphNetProxy::UpdateNodeProxy(const FHeartReplicatedNodeData& NodeData)
{
	if (IsValid(ProxyGraph))
	{
		if (UHeartGraphNode* ExistingNode = ProxyGraph->GetNode(NodeData.NodeGuid))
		{
			Heart::Flakes::WriteObject(ExistingNode, NodeData.FlakeData);
			OnNodeProxyUpdated.Broadcast(ExistingNode);
			return true;
		}

		if (UHeartGraphNode* NewNode = Heart::Flakes::CreateObject<UHeartGraphNode>(NodeData.FlakeData, ProxyGraph))
		{
			ProxyGraph->AddNode(NewNode);
			return true;
		}
	}

	return false;
}

bool UHeartGraphNetProxy::RemoveNodeProxy(const FHeartNodeGuid& Node)
{
	if (IsValid(ProxyGraph))
	{
		return ProxyGraph->RemoveNode(Node);
	}
	return false;
}