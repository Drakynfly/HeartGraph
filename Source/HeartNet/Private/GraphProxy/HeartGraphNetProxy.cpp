// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphProxy/HeartGraphNetProxy.h"
#include "GraphProxy/HeartNetClient.h"
#include "LogHeartNet.h"
#include "GraphProxy/HeartNetReplicationTypes.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNode3D.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNetProxy)

namespace Heart::Net::Tags
{
	UE_DEFINE_GAMEPLAY_TAG(Node_Added, "Heart.Net.NodeAdded")
	UE_DEFINE_GAMEPLAY_TAG(Node_Removed, "Heart.Net.NodeRemoved")
	UE_DEFINE_GAMEPLAY_TAG(Node_Moved, "Heart.Net.NodeMoved")
	UE_DEFINE_GAMEPLAY_TAG(Node_ConnectionsChanged, "Heart.Net.NodeConnectionsChanged")
	UE_DEFINE_GAMEPLAY_TAG(Other, "Heart.Net.OtherEvent")
	UE_DEFINE_GAMEPLAY_TAG(Permission_All, "Heart.Net.AllPermissions")
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

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ClientPermissions, Params);

	DOREPLIFETIME(ThisClass, ReplicatedNodes);

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

UHeartGraphNetProxy* UHeartGraphNetProxy::CreateHeartNetProxy(AActor* Owner, UHeartGraph* SourceGraph, TSubclassOf<UHeartGraphNetProxy> ProxyClassOverride)
{
	ensure(Owner->GetIsReplicated());
	ensure(Owner->IsUsingRegisteredSubObjectList());

	UHeartGraphNetProxy* NewProxy = NewObject<UHeartGraphNetProxy>(Owner, IsValid(ProxyClassOverride) ? ProxyClassOverride.Get() : UHeartGraphNetProxy::StaticClass());

	Owner->AddReplicatedSubObject(NewProxy);
	ensure(Owner->IsReplicatedSubObjectRegistered(NewProxy));

	NewProxy->SetupGraphProxy(SourceGraph);

	return NewProxy;
}

void UHeartGraphNetProxy::SetLocalClient(UHeartNetClient* NetClient)
{
	LocalClient = NetClient;
}

void UHeartGraphNetProxy::Destroy()
{
	if (IsValid(this))
	{
		checkf(GetOwningActor()->HasAuthority() == true, TEXT("[UHeartGraphNetProxy::Destroy] Object does not have authority to destroy itself!"));

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

	Heart::Query::FNodeQueryResult(SourceGraph)
		.Filter_UObject(this, &ThisClass::ShouldReplicateNode)
		.ForEach_UObject(this, &ThisClass::UpdateReplicatedNodeData);

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
			NodeData.Data.Flake = Heart::Flakes::CreateFlake(Node);
			UE_LOG(LogHeartNet, Log, TEXT("Updated replicated node '%s' (%i bytes)"),
				*Node->GetName(), NodeData.Data.Flake.Data.Num());
		});
}

void UHeartGraphNetProxy::RemoveReplicatedNodeData(const FHeartNodeGuid& Node)
{
	ReplicatedNodes.Delete(Node);
}

void UHeartGraphNetProxy::EditReplicatedNodeData(const FHeartNodeFlake& NodeData, const FGameplayTag Type)
{
	checkf(GetOwningActor()->HasAuthority(), TEXT("The source graph can only be edited on the server!"));

	if (!IsValid(SourceGraph))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("[UHeartGraphNetProxy::EditReplicatedNodeData] Invalid Source Graph!"))
		return;
	}

	auto&& ExistingNode = SourceGraph->GetNode(NodeData.Guid);

	// Handle clients trying to add nodes
	if (Type == Heart::Net::Tags::Node_Added)
	{
		if (IsValid(ExistingNode))
		{
			UE_LOG(LogHeartNet, Warning, TEXT("Client edit tried to create node '%s' that already exists in source graph!"), *NodeData.Guid.ToString())
			return;
		}

		if (UHeartGraphNode* NewNode = Heart::Flakes::CreateObject<UHeartGraphNode>(NodeData.Flake, SourceGraph))
		{
			SourceGraph->AddNode(NewNode);
			OnNodeSourceEdited.Broadcast(ExistingNode, Heart::Net::Tags::Node_Added);
		}
		return;
	}

	if (!IsValid(ExistingNode))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client edit tried to edit node '%s' that does not exist in source graph!"), *NodeData.Guid.ToString())
		return;
	}

	// Handle clients trying to move nodes
	if (Type == Heart::Net::Tags::Node_Moved)
	{
		if (UHeartGraphNode3D* Node3D = Cast<UHeartGraphNode3D>(ExistingNode))
		{
			FVector Location;
			Heart::Flakes::WriteStruct<FVector>(Location, NodeData.Flake);
			Node3D->SetLocation3D(Location);
		}
		else
		{
			FVector2D Location;
			Heart::Flakes::WriteStruct<FVector2D>(Location, NodeData.Flake);
			ExistingNode->SetLocation(Location);
		}

		OnNodeSourceEdited.Broadcast(ExistingNode, Heart::Net::Tags::Node_Moved);
		return;
	}

	// Handle clients trying to edit connections
	if (Type == Heart::Net::Tags::Node_ConnectionsChanged)
	{
		FHeartGraphConnectionEvent_Net_PinElement PinElement;
		Heart::Flakes::WriteStruct<FHeartGraphConnectionEvent_Net_PinElement>(PinElement, NodeData.Flake);

		Heart::Connections::FEdit Edit = ExistingNode->GetGraph()->EditConnections();

		for (auto&& Element : PinElement.PinConnections)
		{
			Edit.Override({ NodeData.Guid, Element.Key}, Element.Value);
		}

		OnNodeSourceEdited.Broadcast(ExistingNode, Heart::Net::Tags::Node_ConnectionsChanged);
		return;
	}

	if (Type == Heart::Net::Tags::Other)
	{
		Heart::Flakes::WriteObject(ExistingNode, NodeData.Flake);
		OnNodeSourceEdited.Broadcast(ExistingNode, Heart::Net::Tags::Other);
		return;
	}

	checkf(0, TEXT("This function should always be handled before reaching end."))
}

UHeartGraph* UHeartGraphNetProxy::GetProxiedGraph() const
{
	return ProxyGraph;
}

void UHeartGraphNetProxy::SetPermissions(const FGameplayTagContainer& Permissions)
{
	ClientPermissions = Permissions;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ClientPermissions, this);
}

void UHeartGraphNetProxy::OnRep_GraphClass()
{
	ensure(!ProxyGraph && GraphClass);

	ProxyGraph = NewObject<UHeartGraph>(this, GraphClass);

	ProxyGraph->GetOnNodeAdded().AddUObject(this, &ThisClass::OnNodeAdded_Proxy);
	ProxyGraph->GetOnNodeMoved().AddUObject(this, &ThisClass::OnNodesMoved_Proxy);
	ProxyGraph->GetOnNodeRemoved().AddUObject(this, &ThisClass::OnNodeRemoved_Proxy);
	ProxyGraph->GetOnNodeConnectionsChanged().AddUObject(this, &ThisClass::OnNodeConnectionsChanged_Proxy);

	// Add all existing nodes to proxy graph
	for (auto&& Element : ReplicatedNodes.Items)
	{
		UpdateNodeProxy(Element, Heart::Net::Tags::Node_Added);
	}
}

void UHeartGraphNetProxy::OnRep_ClientPermissions(const FGameplayTagContainer& OldPermissions)
{
}

bool UHeartGraphNetProxy::CanClientPerformEvent(const FGameplayTag RequestedEventType) const
{
	/*
	 * Default logic for determining if the Requested event can be executed. Will succeed if the tag itself or the
	 * AllPermissions tag is present in the ClientPermissions container. This single container is used for all possible
	 * clients. To have unique permissions on a per-client bases, override this function in a child class.
	 */

	const FGameplayTagQuery Query = FGameplayTagQuery::MakeQuery_MatchAnyTags(FGameplayTagContainer::CreateFromArray(
			TArray<FGameplayTag>{RequestedEventType, Heart::Net::Tags::Permission_All}));

	return ClientPermissions.MatchesQuery(Query);
}

void UHeartGraphNetProxy::OnNodeAdded_Proxy(UHeartGraphNode* HeartGraphNode)
{
	if (RecursionGuard) return;

	if (IsValid(LocalClient))
	{
		UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnNodeAdded"))
		LocalClient->OnNodeAdded(this, HeartGraphNode);
	}
}

void UHeartGraphNetProxy::OnNodesMoved_Proxy(const FHeartNodeMoveEvent& NodeMoveEvent)
{
	if (IsValid(LocalClient))
	{
		UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnNodesMoved"))
		LocalClient->OnNodesMoved(this, NodeMoveEvent);
	}
}

void UHeartGraphNetProxy::OnNodeRemoved_Proxy(UHeartGraphNode* HeartGraphNode)
{
	if (IsValid(LocalClient))
	{
		UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnNodeRemoved"))
		LocalClient->OnNodeRemoved(this, HeartGraphNode);
	}
}

void UHeartGraphNetProxy::OnNodeConnectionsChanged_Proxy(const FHeartGraphConnectionEvent& GraphConnectionEvent)
{
	if (IsValid(LocalClient))
	{
		UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnNodeConnectionsChanged"))
		LocalClient->OnNodeConnectionsChanged(this, GraphConnectionEvent);
	}
}

void UHeartGraphNetProxy::OnNodeAdded_Client(const FHeartNodeFlake& HeartGraphNode)
{
	if (!CanClientPerformEvent(Heart::Net::Tags::Node_Added))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to perform illegal event: '%s' on node '%s'"),
			*Heart::Net::Tags::Node_Added.GetTag().ToString(),
			*HeartGraphNode.Guid.ToString())
		return;
	}

	EditReplicatedNodeData(HeartGraphNode, Heart::Net::Tags::Node_Added);
}

void UHeartGraphNetProxy::OnNodesMoved_Client(const FHeartNodeMoveEvent_Net& NodeMoveEvent)
{
	if (!CanClientPerformEvent(Heart::Net::Tags::Node_Moved))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to perform illegal event: '%s'"),
			*Heart::Net::Tags::Node_Moved.GetTag().ToString())
		return;
	}

	for (auto&& Element : NodeMoveEvent.AffectedNodes)
	{
		EditReplicatedNodeData(Element, Heart::Net::Tags::Node_Moved);
	}
}

void UHeartGraphNetProxy::OnNodeRemoved_Client(const FHeartNodeGuid HeartGraphNode)
{
	if (!CanClientPerformEvent(Heart::Net::Tags::Node_Removed))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to perform illegal event: '%s' on node '%s'"),
			*Heart::Net::Tags::Node_Removed.GetTag().ToString(),
			*HeartGraphNode.ToString())
		return;
	}

	if (IsValid(SourceGraph))
	{
		if (!SourceGraph->RemoveNode(HeartGraphNode))
		{
			UE_LOG(LogHeartNet, Warning, TEXT("Client attempt to perform remove node failed: '%s'"),
				*HeartGraphNode.ToString())
		}
	}
}

void UHeartGraphNetProxy::OnNodeConnectionsChanged_Client(const FHeartGraphConnectionEvent_Net& GraphConnectionEvent)
{
	if (!CanClientPerformEvent(Heart::Net::Tags::Node_ConnectionsChanged))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to perform illegal event: '%s'"),
			*Heart::Net::Tags::Node_ConnectionsChanged.GetTag().ToString())
		return;
	}

	for (auto&& Element : GraphConnectionEvent.AffectedNodes)
	{
		EditReplicatedNodeData(Element, Heart::Net::Tags::Node_ConnectionsChanged);
	}
}

bool UHeartGraphNetProxy::UpdateNodeProxy(const FHeartReplicatedNodeData& NodeData, const FGameplayTag EventType)
{
	if (IsValid(ProxyGraph))
	{
		if (UHeartGraphNode* ExistingNode = ProxyGraph->GetNode(NodeData.Data.Guid))
		{
			Heart::Flakes::WriteObject(ExistingNode, NodeData.Data.Flake);
			OnNodeProxyUpdated.Broadcast(ExistingNode, EventType);
			return true;
		}

		if (UHeartGraphNode* NewNode = Heart::Flakes::CreateObject<UHeartGraphNode>(NodeData.Data.Flake, ProxyGraph))
		{
			ensure(EventType == Heart::Net::Tags::Node_Added);

			{
				// Prevent OnNodeAdded_Proxy from pinging this back to the server
				TGuardValue<bool> bRecursionGuard(RecursionGuard, true);
				ProxyGraph->AddNode(NewNode);
			}

			OnNodeProxyUpdated.Broadcast(NewNode, Heart::Net::Tags::Node_Added);
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