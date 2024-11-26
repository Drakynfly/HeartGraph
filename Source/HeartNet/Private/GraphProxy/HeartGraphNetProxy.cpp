// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphProxy/HeartGraphNetProxy.h"
#include "GraphProxy/HeartNetClient.h"
#include "GraphProxy/HeartNetExtensionInterface.h"
#include "GraphProxy/HeartNetNodeInterface.h"
#include "GraphProxy/HeartNetReplicationTypes.h"
#include "Input/HeartActionBase.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNode3D.h"
#include "Model/HeartNodeQuery.h"
#include "Model/HeartExtensionQuery.h"
#include "ModelView/HeartActionHistory.h"
#include "View/HeartVisualizerInterfaces.h"

#include "Actions/HeartRemoteActionLog.h"
#include "LogHeartNet.h"
#include "Providers/FlakesNetBinarySerializer.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNetProxy)

namespace Heart::Net::Tags
{
	UE_DEFINE_GAMEPLAY_TAG(Node_Added, "Heart.Net.NodeAdded")
	UE_DEFINE_GAMEPLAY_TAG(Node_Removed, "Heart.Net.NodeRemoved")
	UE_DEFINE_GAMEPLAY_TAG(Node_Moved, "Heart.Net.NodeMoved")
	UE_DEFINE_GAMEPLAY_TAG(Node_ConnectionsChanged, "Heart.Net.NodeConnectionsChanged")

	UE_DEFINE_GAMEPLAY_TAG(Node_ClientUpdateNodeObject, "Heart.Net.UpdateNodeObject")

	UE_DEFINE_GAMEPLAY_TAG(Extension_Added, "Heart.Net.ExtensionAdded")
	UE_DEFINE_GAMEPLAY_TAG(Extension_Removed, "Heart.Net.ExtensionRemoved")

	UE_DEFINE_GAMEPLAY_TAG(Other, "Heart.Net.OtherEvent")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Permission_Actions, "Heart.Net.GraphActionPermission", "Allows clients to run graph action edits")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Permission_UndoRedo, "Heart.Net.GraphHistoryPermission", "Allows clients to undo and redo graph action edits")
	UE_DEFINE_GAMEPLAY_TAG(Permission_All, "Heart.Net.AllPermissions")
}

UHeartGraphNetProxy::UHeartGraphNetProxy()
{
	ReplicatedNodes.OwningProxy = this;
	ReplicatedExtensions.OwningProxy = this;
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
	DOREPLIFETIME(ThisClass, ReplicatedExtensions);

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

UHeartGraphNetProxy* UHeartGraphNetProxy::CreateHeartNetProxy(AActor* Owner, UHeartGraph* SourceGraph, const TSubclassOf<UHeartGraphNetProxy> ProxyClassOverride)
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
	if (GetOwningActor()->HasAuthority())
	{
		if (ShouldReplicateNode(Node))
		{
			UpdateReplicatedNodeData(Node);
		}
	}
	else
	{
		RequestUpdateNode_OnServer(Node->GetGuid(), EHeartUpdateNodeType::HeartNode);
	}
}

bool UHeartGraphNetProxy::SetupGraphProxy(UHeartGraph* InSourceGraph)
{
	if (!IsValid(InSourceGraph))
	{
		return false;
	}

	SourceGraph = InSourceGraph;

	// Node delegates
	SourceGraph->GetOnNodeAdded().AddUObject(this, &ThisClass::OnNodeAdded_Source);
	SourceGraph->GetOnNodeRemoved().AddUObject(this, &ThisClass::OnNodeRemoved_Source);
	SourceGraph->GetOnNodeMoved().AddUObject(this, &ThisClass::OnNodesMoved_Source);
	SourceGraph->GetOnNodeConnectionsChanged().AddUObject(this, &ThisClass::OnNodeConnectionsChanged_Source);

	// Extension delegates
	SourceGraph->GetOnExtensionAdded().AddUObject(this, &ThisClass::OnExtensionAdded_Source);
	SourceGraph->GetOnExtensionRemoved().AddUObject(this, &ThisClass::OnExtensionRemoved_Source);

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, GraphClass, this);
	GraphClass = SourceGraph->GetClass();

	Heart::Query::TNodeQueryResult<UHeartGraph>(SourceGraph)
		.Filter_UObject(this, &ThisClass::ShouldReplicateNode)
		.ForEach_UObject(this, &ThisClass::UpdateReplicatedNodeData);

	Heart::Query::TExtensionQueryResult<UHeartGraph>(SourceGraph)
		.Filter_UObject(this, &ThisClass::ShouldReplicateExtension)
		.ForEach_UObject(this, &ThisClass::UpdateReplicatedExtensionData);

	return true;
}

void UHeartGraphNetProxy::OnNodeAdded_Source(UHeartGraphNode* HeartGraphNode)
{
	if (ShouldReplicateNode(HeartGraphNode))
	{
		UpdateReplicatedNodeData(HeartGraphNode);
	}
}

void UHeartGraphNetProxy::OnNodeRemoved_Source(UHeartGraphNode* HeartGraphNode)
{
	if (ShouldReplicateNode(HeartGraphNode))
	{
		ReplicatedNodes.Delete(HeartGraphNode->GetGuid());
	}
}

void UHeartGraphNetProxy::OnNodesMoved_Source(const FHeartNodeMoveEvent& NodeMoveEvent)
{
	if (NodeMoveEvent.MoveFinished)
	{
		for (auto Element : NodeMoveEvent.AffectedNodes)
		{
			if (ShouldReplicateNode(Element))
			{
				UpdateReplicatedNodeData(Element);
			}
		}
	}
}

void UHeartGraphNetProxy::OnNodeConnectionsChanged_Source(const FHeartGraphConnectionEvent& GraphConnectionEvent)
{
	for (auto Element : GraphConnectionEvent.AffectedNodes)
	{
		if (ShouldReplicateNode(Element))
		{
			UpdateReplicatedNodeData(Element);
		}
	}
}

void UHeartGraphNetProxy::OnExtensionAdded_Source(UHeartGraphExtension* Extension)
{
	if (ShouldReplicateExtension(Extension))
	{
		UpdateReplicatedExtensionData(Extension);
	}
}

void UHeartGraphNetProxy::OnExtensionRemoved_Source(UHeartGraphExtension* Extension)
{
	ReplicatedExtensions.Delete(Extension->GetGuid());
}

bool UHeartGraphNetProxy::ShouldReplicateNode(const TObjectPtr<UHeartGraphNode> Node) const
{
	if (Node->Implements<UHeartNetNodeInterface>())
	{
		return IHeartNetNodeInterface::Execute_ShouldReplicate(Node);
	}
	return true;
}

bool UHeartGraphNetProxy::ShouldReplicateExtension(const TObjectPtr<UHeartGraphExtension> Extension) const
{
	if (Extension->Implements<UHeartNetExtensionInterface>())
	{
		return IHeartNetExtensionInterface::Execute_ShouldReplicate(Extension);
	}
	return true;
}

void UHeartGraphNetProxy::UpdateReplicatedNodeData(TObjectPtr<UHeartGraphNode> Node)
{
	if (!IsValid(Node)) return;

	ReplicatedNodes.Operate(Node->GetGuid(),
		[Node](FHeartReplicatedFlake& Data)
		{
			Data.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(Node);
			UE_LOG(LogHeartNet, Log, TEXT("Updated replicated node '%s' (%i bytes)"),
				*Node->GetName(), Data.Flake.Data.Num());
		});
}

void UHeartGraphNetProxy::UpdateReplicatedExtensionData(TObjectPtr<UHeartGraphExtension> Extension)
{
	if (!IsValid(Extension)) return;

	ReplicatedExtensions.Operate(Extension->GetGuid(),
		[Extension](FHeartReplicatedFlake& Data)
		{
			Data.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(Extension);
			UE_LOG(LogHeartNet, Log, TEXT("Updated replicated extension '%s' (%i bytes)"),
				*Extension->GetName(), Data.Flake.Data.Num());
		});
}

void UHeartGraphNetProxy::EditReplicatedNodeData(const FHeartReplicatedFlake& NodeData, const FGameplayTag EventType)
{
	checkf(GetOwningActor()->HasAuthority(), TEXT("The source graph can only be edited on the server!"));

	if (!IsValid(SourceGraph))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("[UHeartGraphNetProxy::EditReplicatedNodeData] Invalid Source Graph!"))
		return;
	}

	auto&& ExistingNode = SourceGraph->GetNode(NodeData.Guid.Get<FHeartNodeGuid>());

	// Handle clients trying to add nodes
	if (EventType == Heart::Net::Tags::Node_Added)
	{
		if (IsValid(ExistingNode))
		{
			UE_LOG(LogHeartNet, Warning, TEXT("Client edit tried to create node '%s' that already exists in source graph!"), *NodeData.Guid.ToString())
			return;
		}

		if (UHeartGraphNode* NewNode = Flakes::CreateObject<UHeartGraphNode, Flakes::NetBinary::Type>(NodeData.Flake, SourceGraph))
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
	if (EventType == Heart::Net::Tags::Node_Moved)
	{
		if (UHeartGraphNode3D* Node3D = Cast<UHeartGraphNode3D>(ExistingNode))
		{
			const FVector Location = Flakes::CreateStruct<Flakes::NetBinary::Type, FVector>(NodeData.Flake);
			Node3D->SetLocation3D(Location);
		}
		else
		{
			const FVector2D Location = Flakes::CreateStruct<Flakes::NetBinary::Type, FVector2D>(NodeData.Flake);
			ExistingNode->SetLocation(Location);
		}

		OnNodeSourceEdited.Broadcast(ExistingNode, Heart::Net::Tags::Node_Moved);
		return;
	}

	// Handle clients trying to edit connections
	if (EventType == Heart::Net::Tags::Node_ConnectionsChanged)
	{
		FHeartGraphConnectionEvent_Net_PinElement PinElement;
		Flakes::WriteStruct<Flakes::NetBinary::Type>(FStructView::Make(PinElement), NodeData.Flake);

		Heart::Connections::FEdit Edit(ExistingNode);

		for (auto&& Element : PinElement.PinConnections)
		{
			Edit.Override({ NodeData.Guid.Get<FHeartNodeGuid>(), Element.Key}, Element.Value);
		}

		OnNodeSourceEdited.Broadcast(ExistingNode, Heart::Net::Tags::Node_ConnectionsChanged);
		return;
	}

	if (EventType == Heart::Net::Tags::Node_ClientUpdateNodeObject)
	{
		if (ExistingNode->GetNodeObject()->GetOuter() != ExistingNode)
		{
			UE_LOG(LogHeartNet, Warning, TEXT("Attempted to write to a non-instanced NodeObject!"))
			return;
		}
		Flakes::WriteObject<Flakes::NetBinary::Type>(ExistingNode->GetNodeObject(), NodeData.Flake);
		OnNodeSourceEdited.Broadcast(ExistingNode, Heart::Net::Tags::Node_ClientUpdateNodeObject);
		return;
	}

	if (EventType == Heart::Net::Tags::Other)
	{
		Flakes::WriteObject<Flakes::NetBinary::Type>(ExistingNode, NodeData.Flake);
		OnNodeSourceEdited.Broadcast(ExistingNode, Heart::Net::Tags::Other);
		return;
	}

	checkf(0, TEXT("This function should always be handled before reaching end."))
}

bool UHeartGraphNetProxy::IsProxyGraph(const UHeartGraph* Graph)
{
	return IsValid(Graph) && IsValid(Graph->GetTypedOuter<UHeartGraphNetProxy>());
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

void UHeartGraphNetProxy::RequestUpdateNode_OnServer(const FHeartNodeGuid& NodeGuid, const EHeartUpdateNodeType Type)
{
	if (!ensure(NodeGuid.IsValid()))
	{
		return;
	}

	FHeartReplicatedFlake NodeFlake;
	NodeFlake.Guid = NodeGuid;

	switch (Type)
	{
	case EHeartUpdateNodeType::None:
		break;
	case EHeartUpdateNodeType::HeartNode:
		NodeFlake.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(ProxyGraph->GetNode(NodeGuid));
		break;
	case EHeartUpdateNodeType::NodeObject:
		NodeFlake.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(ProxyGraph->GetNode(NodeGuid)->GetNodeObject());
		break;
	}

	LocalClient->Server_UpdateGraphNode(this, NodeFlake, Type);
}

void UHeartGraphNetProxy::ExecuteGraphActionOnServer(const TSubclassOf<UHeartActionBase> Action, UObject* Target, const FHeartManualEvent& Activation, UObject* ContextObject)
{
	if (!ensure(IsValid(Action)))
	{
		return;
	}
	if (!ensure(IsValid(Target)))
	{
		return;
	}
	if (!ensure(IsValid(LocalClient)))
	{
		return;
	}

	FHeartRemoteGraphActionArguments Args;

	if (Target->Implements<UGraphNodeVisualizerInterface>())
	{
		Args.NodeAndPinGuid.NodeGuid = IGraphNodeVisualizerInterface::Execute_GetVisualizingNode(Target)->GetGuid();
	}
	else if (const IHeartGraphNodeInterface* NodeInterface = Cast<IHeartGraphNodeInterface>(Target))
	{
		Args.NodeAndPinGuid.NodeGuid = NodeInterface->GetHeartGraphNode()->GetGuid();
	}
	else
	{
		if (Target->Implements<UGraphPinVisualizerInterface>())
		{
			Args.PinTarget = Target;
		}
		else if (const IHeartGraphPinInterface* PinInterface = Cast<IHeartGraphPinInterface>(Target))
		{
			Args.NodeAndPinGuid.NodeGuid = PinInterface->GetHeartGraphNode()->GetGuid();
			Args.NodeAndPinGuid.PinGuid = PinInterface->GetPinGuid();
			Args.PinTarget = Target;
		}
	}

	Args.Activation = Activation;
	Args.ContextObject = ContextObject;

	if (LogActionsClientside)
	{
		if (Heart::Action::CanUndo(Action, Target))
		{
			if (auto&& History = ProxyGraph->GetExtension<UHeartActionHistory>();
				IsValid(History))
			{
				FHeartRemoteActionLogUndoData UndoData;
				UndoData.NetProxy = this;

				FHeartActionRecord Record;
				Record.Action = UHeartRemoteActionLog::StaticClass();
				Record.UndoData.Add(UHeartRemoteActionLog::LogStorage, UndoData);
				History->AddRecord(Record);
			}
		}
	}

	UE_LOG(LogHeartNet, Log, TEXT("Proxy: ExecuteGraphAction"))
	LocalClient->Server_ExecuteGraphAction(this, Action, Args);
}

void UHeartGraphNetProxy::ExecuteUndoOnServer()
{
	UE_LOG(LogHeartNet, Log, TEXT("Proxy: UndoGraphAction"))
	LocalClient->Server_UndoGraphAction(this);
}

void UHeartGraphNetProxy::ExecuteRedoOnServer()
{
	UE_LOG(LogHeartNet, Log, TEXT("Proxy: RedoGraphAction"))
	LocalClient->Server_RedoGraphAction(this);
}

void UHeartGraphNetProxy::OnRep_GraphClass()
{
	ensure(!ProxyGraph && GraphClass);

	ProxyGraph = NewObject<UHeartGraph>(this, GraphClass);

	ProxyGraph->GetOnNodeAdded().AddUObject(this, &ThisClass::OnNodeAdded_Proxy);
	ProxyGraph->GetOnNodeMoved().AddUObject(this, &ThisClass::OnNodesMoved_Proxy);
	ProxyGraph->GetOnNodeRemoved().AddUObject(this, &ThisClass::OnNodeRemoved_Proxy);
	ProxyGraph->GetOnNodeConnectionsChanged().AddUObject(this, &ThisClass::OnNodeConnectionsChanged_Proxy);

	// @todo annoying, but we need to clear the local copies of extensions added by the schema if the server is going to
	// replicate its versions to us. Only remove replicated extensions. This is necessary since the guids on the server
	// will not initially match the ones that are generated by the schema, as FHeartExtensionGuid (and all guids) are
	// considered globally unique, not just per-graph unique. If that is changed (hmm), this would not be needed, as the
	// replication fragments would find the extension by matching guids.
	Heart::Query::TExtensionQueryResult<UHeartGraph>(ProxyGraph)
		.Filter_UObject(this, &ThisClass::ShouldReplicateExtension)
		.ForEach([this](const FHeartExtensionGuid Guid)
		{
			ProxyGraph->RemoveExtension(Guid);
		});

	// Add all existing nodes to the proxy graph
	for (auto&& Element : ReplicatedNodes.Items)
	{
		UpdateNodeProxy(Element, Heart::Net::Tags::Node_Added);
	}

	// Add all existing nodes to the proxy graph
	for (auto&& Element : ReplicatedExtensions.Items)
	{
		UpdateExtensionProxy(Element, Heart::Net::Tags::Extension_Added);
	}
}

void UHeartGraphNetProxy::OnRep_ClientPermissions(const FGameplayTagContainer& OldPermissions)
{
}

bool UHeartGraphNetProxy::CanClientPerformEvent(const FGameplayTag RequestedEventType) const
{
	if (!ensure(GetOwningActor()->HasAuthority()))
	{
		return false;
	}

	/*
	 * Default logic for determining if the Requested event can be executed. Will succeed if the tag itself or the
	 * AllPermissions tag is present in the ClientPermissions container. This single container is used for all possible
	 * clients. To have unique permissions on a per-client basis, override this function in a child class.
	 */

	const FGameplayTagQuery Query = FGameplayTagQuery::MakeQuery_MatchAnyTags(FGameplayTagContainer::CreateFromArray(
			TArray<FGameplayTag>{RequestedEventType, Heart::Net::Tags::Permission_All}));

	return ClientPermissions.MatchesQuery(Query);
}

void UHeartGraphNetProxy::OnNodeAdded_Proxy(UHeartGraphNode* HeartGraphNode)
{
	if (RecursionGuards[NodeAdd]) return;

	if (!IsValid(LocalClient))
	{
		return;
	}

	UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnNodeAdded"))

	ensure(LocalClient->GetOwnerRole() == ROLE_AutonomousProxy);

	FHeartReplicatedFlake NodeData;
	NodeData.Guid = HeartGraphNode->GetGuid();
	NodeData.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(HeartGraphNode);
	UE_LOG(LogHeartNet, Log, TEXT("Sending node RPC data '%s' (%i bytes)"),
		*HeartGraphNode->GetName(), NodeData.Flake.Data.Num());

	LocalClient->Server_OnNodeAdded(this, NodeData);
}

void UHeartGraphNetProxy::OnNodeRemoved_Proxy(UHeartGraphNode* HeartGraphNode)
{
	if (RecursionGuards[NodeDelete]) return;

	if (!IsValid(LocalClient))
	{
		return;
	}

	UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnNodeRemoved"))
	ensure(LocalClient->GetOwnerRole() == ROLE_AutonomousProxy);

	LocalClient->Server_OnNodeRemoved(this, HeartGraphNode->GetGuid());
}

void UHeartGraphNetProxy::OnNodesMoved_Proxy(const FHeartNodeMoveEvent& NodeMoveEvent)
{
	if (!IsValid(LocalClient))
	{
		return;
	}

	if (NodeMoveEvent.MoveFinished)
	{
		UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnNodesMoved"))
		ensure(LocalClient->GetOwnerRole() == ROLE_AutonomousProxy);

		FHeartNodeMoveEvent_Net Event;
		Algo::Transform(NodeMoveEvent.AffectedNodes, Event.AffectedNodes,
			[](const TObjectPtr<UHeartGraphNode>& Node)
			{
				FHeartReplicatedFlake NodeData;
				NodeData.Guid = Node->GetGuid();
				if (auto&& Node3D = Cast<UHeartGraphNode3D>(Node))
				{
					NodeData.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(FConstStructView::Make(Node3D->GetLocation3D()));
				}
				else
				{
					NodeData.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(FConstStructView::Make(Node->GetLocation()));
				}

				UE_LOG(LogHeartNet, Log, TEXT("Sending node RPC data '%s' (%i bytes)"),
					*Node->GetName(), NodeData.Flake.Data.Num());
				return NodeData;
			});

		Event.MoveFinished = NodeMoveEvent.MoveFinished;

		LocalClient->Server_OnNodesMoved(this, Event);
	}
}

void UHeartGraphNetProxy::OnNodeConnectionsChanged_Proxy(const FHeartGraphConnectionEvent& GraphConnectionEvent)
{
	if (!IsValid(LocalClient))
	{
		return;
	}

	UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnNodeConnectionsChanged"))
	ensure(LocalClient->GetOwnerRole() == ROLE_AutonomousProxy);

	auto ByAffected = [&GraphConnectionEvent](const FHeartPinGuid Pin)
		{
			return GraphConnectionEvent.AffectedPins.Contains(Pin);
		};

	FHeartGraphConnectionEvent_Net Event;
	Algo::Transform(GraphConnectionEvent.AffectedNodes, Event.AffectedNodes,
		[&ByAffected](const TObjectPtr<UHeartGraphNode>& Node)
		{
			FHeartReplicatedFlake NodeData;
			NodeData.Guid = Node->GetGuid();

			FHeartGraphConnectionEvent_Net_PinElement PinElement;
			Node->QueryPins()
				.Filter(ByAffected)
				.ForEach([Node, &PinElement](const FHeartPinGuid Pin)
				{
					if (auto&& Connections = Node->GetConnections(Pin);
						Connections.IsSet())
					{
						PinElement.PinConnections.Add(Pin, Connections.GetValue());
					}
					else
					{
						PinElement.PinConnections.Add(Pin,  FHeartGraphPinConnections{});
					}
				});

			NodeData.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(FConstStructView::Make(PinElement));

			UE_LOG(LogHeartNet, Log, TEXT("Sending node RPC data '%s' (%i bytes)"),
				*Node->GetName(), NodeData.Flake.Data.Num());

			return NodeData;
		});

	LocalClient->Server_OnNodeConnectionsChanged(this, Event);
}

void UHeartGraphNetProxy::OnNodeAdded_Client(const FHeartReplicatedFlake& NodeData)
{
	if (!CanClientPerformEvent(Heart::Net::Tags::Node_Added))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to perform illegal event: '%s' on node '%s'"),
			*Heart::Net::Tags::Node_Added.GetTag().ToString(),
			*NodeData.Guid.ToString())
		return;
	}

	EditReplicatedNodeData(NodeData, Heart::Net::Tags::Node_Added);
}

void UHeartGraphNetProxy::OnNodeRemoved_Client(const FHeartNodeGuid& NodeGuid)
{
	if (!CanClientPerformEvent(Heart::Net::Tags::Node_Removed))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to perform illegal event: '%s' on node '%s'"),
			*Heart::Net::Tags::Node_Removed.GetTag().ToString(),
			*NodeGuid.ToString())
		return;
	}

	if (IsValid(SourceGraph))
	{
		if (!SourceGraph->RemoveNode(NodeGuid))
		{
			UE_LOG(LogHeartNet, Warning, TEXT("Client attempt to perform remove node failed: '%s'"),
				*NodeGuid.ToString())
		}
	}
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

void UHeartGraphNetProxy::UpdateNodeData_Client(const FHeartReplicatedFlake& NodeData, const FGameplayTag EventType)
{
	if (!CanClientPerformEvent(EventType))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to perform illegal event: '%s'"), *EventType.ToString())
		return;
	}

	EditReplicatedNodeData(NodeData, Heart::Net::Tags::Node_ConnectionsChanged);
}

void UHeartGraphNetProxy::ExecuteGraphAction_Client(const TSubclassOf<UHeartActionBase> Action, const FHeartRemoteGraphActionArguments& Args)
{
	if (!CanClientPerformEvent(Heart::Net::Tags::Permission_Actions))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to execute graph action: '%s'"),
			*Heart::Net::Tags::Node_ConnectionsChanged.GetTag().ToString())
		return;
	}

	if (!IsValid(Action))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Graph Action data sent from client failed deserialization from flake!"))
		return;
	}

	UObject* Target = nullptr;

	if (!Args.NodeAndPinGuid.NodeGuid.IsValid())
	{
		// If there is no node target, then the only possible target is the graph itself
		Target = SourceGraph;
	}
	else if (!Args.NodeAndPinGuid.PinGuid.IsValid())
	{
		// If there is no pin target, then the next possible target is a node
		Target = SourceGraph->GetNode(Args.NodeAndPinGuid.NodeGuid);
	}
	else
	{
		Target = Args.PinTarget;
	}

	if (!Heart::Action::Execute(Action, Target, Args.Activation).WasEventSuccessful())
	{
		UE_LOG(LogHeartNet, Log, TEXT("Graph Action data received from client, but Execute failed."))
		return;
	}
}

void UHeartGraphNetProxy::ExecuteUndo_Client()
{
	if (!CanClientPerformEvent(Heart::Net::Tags::Permission_UndoRedo))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to undo action!"))
		return;
	}

	Heart::Action::History::TryUndo(SourceGraph);
}

void UHeartGraphNetProxy::ExecuteRedo_Client()
{
	if (!CanClientPerformEvent(Heart::Net::Tags::Permission_UndoRedo))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to redo action!"))
		return;
	}

	Heart::Action::History::TryRedo(SourceGraph);
}

bool UHeartGraphNetProxy::UpdateNodeProxy(const FHeartReplicatedFlake& Data, const FGameplayTag EventType)
{
	if (IsValid(ProxyGraph))
	{
		if (UHeartGraphNode* ExistingNode = ProxyGraph->GetNode(Data.Guid.Get<FHeartNodeGuid>()))
		{
			Flakes::WriteObject<Flakes::NetBinary::Type>(ExistingNode, Data.Flake);
			OnNodeProxyUpdated.Broadcast(ExistingNode, EventType);
			return true;
		}

		if (UHeartGraphNode* NewNode = Flakes::CreateObject<UHeartGraphNode, Flakes::NetBinary::Type>(Data.Flake, ProxyGraph))
		{
			ensure(EventType == Heart::Net::Tags::Node_Added);

			{
				// Prevent OnNodeAdded_Proxy from pinging this back to the server
				TGuardValue<bool> bRecursionGuard(RecursionGuards[NodeAdd], true);
				ProxyGraph->AddNode(NewNode);
			}

			OnNodeProxyUpdated.Broadcast(NewNode, Heart::Net::Tags::Node_Added);
			return true;
		}
	}

	return false;
}

bool UHeartGraphNetProxy::RemoveNodeProxy(const FHeartNodeGuid& Guid)
{
	if (IsValid(ProxyGraph))
	{
		// No Node to delete for some reason, maybe this is just a ping-back from a node we locally deleted.
		if (!ProxyGraph->GetNode(Guid))
		{
			return false;
		}

		OnNodeProxyUpdated.Broadcast(ProxyGraph->GetNode(Guid), Heart::Net::Tags::Node_Removed);
		bool Result;
		{
			// Prevent OnNodeRemoved_Proxy from pinging this back to the server
			TGuardValue<bool> bRecursionGuard(RecursionGuards[NodeDelete], true);
			Result = ProxyGraph->RemoveNode(Guid);
		}
		return Result;
	}
	return false;
}

bool UHeartGraphNetProxy::UpdateExtensionProxy(const FHeartReplicatedFlake& Data, const FGameplayTag EventType)
{
	if (IsValid(ProxyGraph))
	{
		if (UHeartGraphExtension* ExistingExtension = ProxyGraph->GetExtensionByGuid(Data.Guid.Get<FHeartExtensionGuid>()))
		{
			Flakes::WriteObject<Flakes::NetBinary::Type>(ExistingExtension, Data.Flake);
			OnExtensionProxyUpdated.Broadcast(ExistingExtension, EventType);
			return true;
		}

		if (UHeartGraphExtension* NewExtension = Flakes::CreateObject<UHeartGraphExtension, Flakes::NetBinary::Type>(Data.Flake, ProxyGraph))
		{
			ensure(EventType == Heart::Net::Tags::Extension_Added);

			{
				// Prevent OnExtensionAdded_Proxy from pinging this back to the server
				TGuardValue<bool> bRecursionGuard(RecursionGuards[ExtAdd], true);
				ProxyGraph->AddExtensionInstance(NewExtension);
			}

			OnExtensionProxyUpdated.Broadcast(NewExtension, Heart::Net::Tags::Extension_Added);
			return true;
		}
	}

	return false;
}

bool UHeartGraphNetProxy::RemoveExtensionProxy(const FHeartExtensionGuid& Guid)
{
	if (IsValid(ProxyGraph))
	{
		OnExtensionProxyUpdated.Broadcast(ProxyGraph->GetExtensionByGuid(Guid), Heart::Net::Tags::Extension_Removed);
		bool Result;
		{
			// Prevent OnExtensionRemoved_Proxy from pinging this back to the server
			TGuardValue<bool> bRecursionGuard(RecursionGuards[ExtDelete], true);
			Result = ProxyGraph->RemoveExtension(Guid);
		}
		return Result;
	}
	return false;
}

bool UHeartGraphNetProxy::PostReplicatedAdd(const FHeartReplicatedData& Array, const FHeartReplicatedFlake& Flake)
{
	if (&Array == &ReplicatedNodes)
	{
		UpdateNodeProxy(Flake, Heart::Net::Tags::Node_Added);
		return true;
	}

	if (&Array == &ReplicatedExtensions)
	{
		UpdateExtensionProxy(Flake, Heart::Net::Tags::Extension_Added);
		return true;
	}

	return false;
}

bool UHeartGraphNetProxy::PostReplicatedChange(const FHeartReplicatedData& Array, const FHeartReplicatedFlake& Flake)
{
	if (&Array == &ReplicatedNodes)
	{
		// @todo clients have no idea what kind of changes the server is making...
		UpdateNodeProxy(Flake, Heart::Net::Tags::Other);
		return true;
	}

	if (&Array == &ReplicatedExtensions)
	{
		// @todo clients have no idea what kind of changes the server is making...
		UpdateExtensionProxy(Flake, Heart::Net::Tags::Other);
		return true;
	}

	return false;
}

bool UHeartGraphNetProxy::PreReplicatedRemove(const FHeartReplicatedData& Array, const FHeartReplicatedFlake& Flake)
{
	if (&Array == &ReplicatedNodes)
	{
		RemoveNodeProxy(Flake.Guid.Get<FHeartNodeGuid>());
		return true;
	}

	if (&Array == &ReplicatedExtensions)
	{
		RemoveExtensionProxy(Flake.Guid.Get<FHeartExtensionGuid>());
		return true;
	}

	return false;
}