// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphProxy/HeartGraphNetProxy.h"
#include "GraphProxy/HeartNetClient.h"
#include "GraphProxy/HeartNetExtensionInterface.h"
#include "GraphProxy/HeartNetNodeInterface.h"
#include "GraphProxy/HeartNetReplicationTypes.h"
#include "Input/HeartActionBase.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartNodeQuery.h"
#include "Model/HeartExtensionQuery.h"
#include "ModelView/HeartActionHistory.h"
#include "View/HeartVisualizerInterfaces.h"

#include "Actions/HeartRemoteActionLog.h"
#include "LogHeartNet.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/NetDriver.h"
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

	UE_DEFINE_GAMEPLAY_TAG(NodeComponent_Added, "Heart.Net.NodeComponentAdded")
	UE_DEFINE_GAMEPLAY_TAG(NodeComponent_Removed, "Heart.Net.NodeComponentRemoved")

	UE_DEFINE_GAMEPLAY_TAG(Other, "Heart.Net.OtherEvent")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Permission_Actions, "Heart.Net.GraphActionPermission", "Allows clients to run graph action edits")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Permission_UndoRedo, "Heart.Net.GraphHistoryPermission", "Allows clients to undo and redo graph action edits")
	UE_DEFINE_GAMEPLAY_TAG(Permission_All, "Heart.Net.AllPermissions")
}

UHeartGraphNetProxy::UHeartGraphNetProxy()
{
	ReplicatedNodes.OwningProxy = this;
	ReplicatedExtensions.OwningProxy = this;
	ReplicatedNodeComponents.OwningProxy = this;
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
	DOREPLIFETIME(ThisClass, ReplicatedNodeComponents);

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
	SourceGraph->GetOnNodeAddOrRemove().AddUObject(this, &ThisClass::OnNodeAddedOrRemoved_Source);
	SourceGraph->GetOnNodeMoved().AddUObject(this, &ThisClass::OnNodesMoved_Source);
	SourceGraph->GetOnNodeConnectionsChanged().AddUObject(this, &ThisClass::OnNodeConnectionsChanged_Source);

	// Extension and Component delegates
	SourceGraph->GetOnExtensionAddOrRemove().AddUObject(this, &ThisClass::OnExtensionAddedOrRemoved_Source);
	SourceGraph->GetOnNodeComponentAddOrRemove().AddUObject(this, &ThisClass::OnNodeComponentAddedOrRemoved_Source);

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, GraphClass, this);
	GraphClass = SourceGraph->GetClass();

	Heart::Query::TNodeQueryResult<UHeartGraph>(SourceGraph)
		.Filter_UObject(this, &ThisClass::ShouldReplicateNode)
		.ForEach_UObject(this, &ThisClass::UpdateReplicatedNodeData);

	Heart::Query::TExtensionQueryResult<UHeartGraph>(SourceGraph)
		.Filter_UObject(this, &ThisClass::ShouldReplicateExtension)
		.ForEach_UObject(this, &ThisClass::UpdateReplicatedExtensionData);

	// @todo replace with Query type like above
	for (auto&& Node : SourceGraph->GetNodes())
	{
		for (auto&& Component : SourceGraph->GetNodeComponentsForNode(Node.Key))
        {
        	if (ShouldReplicateNodeComponent(Node.Key, Component))
        	{
        		UpdateReplicatedNodeComponentData(Node.Key, Component);
        	}
        }
	}

	return true;
}

void UHeartGraphNetProxy::OnNodeAddedOrRemoved_Source(const FHeartNodeAddOrRemoveEvent& AddOrRemoveEvent)
{
	switch (AddOrRemoveEvent.Type)
	{
	case EHeartNodeAddOrRemoveEventType::Add:
		for (auto&& Node : AddOrRemoveEvent.Nodes)
		{
			auto&& GraphNode = SourceGraph->GetNode(Node);
			if (ShouldReplicateNode(GraphNode))
			{
				UpdateReplicatedNodeData(GraphNode);
			}
		}
		break;
	case EHeartNodeAddOrRemoveEventType::Remove:
		for (auto&& Node : AddOrRemoveEvent.Nodes)
		{
			auto&& GraphNode = SourceGraph->GetNode(Node);
			if (ShouldReplicateNode(GraphNode))
			{
				ReplicatedNodes.Delete(GraphNode->GetGuid());
			}
		}
		break;
	}
}

void UHeartGraphNetProxy::OnNodesMoved_Source(const FHeartNodeMoveEvent& NodeMoveEvent)
{
	if (NodeMoveEvent.MoveFinished)
	{
		for (auto Element : NodeMoveEvent.AffectedNodes)
		{
			UHeartGraphNode* Node = SourceGraph->GetNode(Element);
			if (ShouldReplicateNode(Node))
			{
				UpdateReplicatedNodeData(Node);
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

void UHeartGraphNetProxy::OnExtensionAddedOrRemoved_Source(UHeartGraphExtension* Extension, const Heart::Events::EComponentEventType Type)
{
	switch (Type)
	{
	case Heart::Events::Add:
		if (ShouldReplicateExtension(Extension))
		{
			UpdateReplicatedExtensionData(Extension);
		}
		break;
	case Heart::Events::Remove:
		ReplicatedExtensions.Delete(Extension->GetGuid());
		break;
	}
}

void UHeartGraphNetProxy::OnNodeComponentAddedOrRemoved_Source(const FHeartNodeGuid& Node,
	UHeartGraphNodeComponent* Component, const Heart::Events::EComponentEventType Type)
{
	switch (Type)
	{
	case Heart::Events::Add:
		if (ShouldReplicateNodeComponent(Node, Component))
		{
			UpdateReplicatedNodeComponentData(Node, Component);
		}
		break;
	case Heart::Events::Remove:
		ReplicatedNodeComponents.Delete(Component->GetGuid());
		break;
	}
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

bool UHeartGraphNetProxy::ShouldReplicateNodeComponent(const FHeartNodeGuid& Node,
	UHeartGraphNodeComponent* Component) const
{
	if (Component->Implements<UHeartNetExtensionInterface>())
	{
		return IHeartNetExtensionInterface::Execute_ShouldReplicate(Component);
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
			UE_LOG(LogHeartNet, Log, TEXT("Updated replicated node '%s' (%llu bytes)"),
				*Node->GetName(), Data.Flake.Data.NumBytes());
		});
}

void UHeartGraphNetProxy::UpdateReplicatedExtensionData(TObjectPtr<UHeartGraphExtension> Extension)
{
	if (!IsValid(Extension)) return;

	ReplicatedExtensions.Operate(Extension->GetGuid(),
		[Extension](FHeartReplicatedFlake& Data)
		{
			Data.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(Extension);
			UE_LOG(LogHeartNet, Log, TEXT("Updated replicated extension '%s' (%llu bytes)"),
				*Extension->GetName(), Data.Flake.Data.NumBytes());
		});
}

void UHeartGraphNetProxy::UpdateReplicatedNodeComponentData(const FHeartNodeGuid& Node, const UHeartGraphNodeComponent* Component)
{
	if (!IsValid(Component)) return;

	ReplicatedNodeComponents.Operate(Component->GetGuid(),
		[&](FHeartReplicatedNodeComponent& Data)
		{
			Data.NodeGuid = Node;
			Data.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(Component);
			UE_LOG(LogHeartNet, Log, TEXT("Updated replicated node Component '%s' (%llu bytes)"),
				*Component->GetName(), Data.Flake.Data.NumBytes());
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
			// @todo replace this with node data export/import
			PRAGMA_DISABLE_DEPRECATION_WARNINGS
			SourceGraph->AddNode(NewNode);
			PRAGMA_ENABLE_DEPRECATION_WARNINGS
			OnNodeSourceEdited.Broadcast(NewNode, EventType);
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
		IHeartNodeLocationInterface* LocationInterface = SourceGraph->GetNodeLocationInterface();

		if (IHeartGraphInterface3D* Interface3D = Cast<IHeartGraphInterface3D>(LocationInterface))
		{
			const FVector Location = Flakes::CreateStruct<Flakes::NetBinary::Type, FVector>(NodeData.Flake);
			Interface3D->SetNodeLocation3D(ExistingNode->GetGuid(), Location, false);
		}
		else
		{
			const FVector2D Location = Flakes::CreateStruct<Flakes::NetBinary::Type, FVector2D>(NodeData.Flake);
			LocationInterface->SetNodeLocation(ExistingNode->GetGuid(), Location, false);
		}

		OnNodeSourceEdited.Broadcast(ExistingNode, EventType);
		return;
	}

	// Handle clients trying to edit connections
	if (EventType == Heart::Net::Tags::Node_ConnectionsChanged)
	{
		FHeartGraphConnectionEvent_Net_PinElement PinElement;
		Flakes::WriteStruct<Flakes::NetBinary::Type>(PinElement, NodeData.Flake, nullptr);

		Heart::API::FPinEdit Edit(SourceGraph);

		for (auto&& Element : PinElement.PinConnections)
		{
			Edit.Override({ NodeData.Guid.Get<FHeartNodeGuid>(), Element.Key}, Element.Value);
		}

		OnNodeSourceEdited.Broadcast(ExistingNode, EventType);
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
		OnNodeSourceEdited.Broadcast(ExistingNode, EventType);
		return;
	}

	if (EventType == Heart::Net::Tags::Other)
	{
		Flakes::WriteObject<Flakes::NetBinary::Type>(ExistingNode, NodeData.Flake);
		OnNodeSourceEdited.Broadcast(ExistingNode, EventType);
		return;
	}

	checkf(0, TEXT("This function should always be handled before reaching end."))
}

void UHeartGraphNetProxy::EditReplicatedExtensionData(const FHeartReplicatedFlake& ExtensionData, const FGameplayTag EventType)
{
	checkf(GetOwningActor()->HasAuthority(), TEXT("The source graph can only be edited on the server!"));

	if (!IsValid(SourceGraph))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("[UHeartGraphNetProxy::EditReplicatedExtensionData] Invalid Source Graph!"))
		return;
	}

	auto&& ExistingExtension = SourceGraph->GetExtensionByGuid(ExtensionData.Guid.Get<FHeartExtensionGuid>());

	// Handle clients trying to add extensions
	if (EventType == Heart::Net::Tags::Extension_Added)
	{
		if (IsValid(ExistingExtension))
		{
			UE_LOG(LogHeartNet, Warning, TEXT("Client edit tried to create extension '%s' that already exists in source graph!"), *ExtensionData.Guid.ToString())
			return;
		}

		if (UHeartGraphExtension* NewExtension = Flakes::CreateObject<UHeartGraphExtension, Flakes::NetBinary::Type>(ExtensionData.Flake, SourceGraph))
		{
			SourceGraph->AddExtensionInstance(NewExtension);
			OnExtensionSourceEdited.Broadcast(NewExtension, EventType);
		}
		return;
	}

	if (!IsValid(ExistingExtension))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client edit tried to edit extension '%s' that does not exist in source graph!"), *ExtensionData.Guid.ToString())
		return;
	}

	if (EventType == Heart::Net::Tags::Other)
	{
		Flakes::WriteObject<Flakes::NetBinary::Type>(ExistingExtension, ExtensionData.Flake);
		OnExtensionSourceEdited.Broadcast(ExistingExtension, EventType);
		return;
	}

	checkf(0, TEXT("This function should always be handled before reaching end."))
}

void UHeartGraphNetProxy::EditReplicatedNodeComponentData(const FHeartReplicatedNodeComponent& ComponentData, const FGameplayTag EventType)
{
	checkf(GetOwningActor()->HasAuthority(), TEXT("The source graph can only be edited on the server!"));

	if (!IsValid(SourceGraph))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("[UHeartGraphNetProxy::EditReplicatedNodeComponentData] Invalid Source Graph!"))
		return;
	}

	auto&& ExistingComponent = SourceGraph->FindNodeComponentByGuid(ComponentData.NodeGuid, ComponentData.ComponentGuid);

	// Handle clients trying to add node components
	if (EventType == Heart::Net::Tags::NodeComponent_Added)
	{
		if (IsValid(ExistingComponent))
		{
			UE_LOG(LogHeartNet, Warning, TEXT("Client edit tried to create node component '%s' that already exists in source graph!"), *ComponentData.ComponentGuid.ToString())
			return;
		}

		UClass* ComponentClass = Cast<UClass>(ComponentData.Flake.Struct.TryLoad());
		if (UHeartGraphNodeComponent* NewComponent = SourceGraph->FindOrAddNodeComponent(ComponentData.NodeGuid, ComponentClass))
		{
			Flakes::WriteObject<Flakes::NetBinary::Type>(NewComponent, ComponentData.Flake);
			OnNodeComponentSourceEdited.Broadcast(ComponentData.NodeGuid, NewComponent, EventType);
		}
		return;
	}

	if (!IsValid(ExistingComponent))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client edit tried to edit node component '%s' that does not exist in source graph!"), *ComponentData.ComponentGuid.ToString())
		return;
	}

	if (EventType == Heart::Net::Tags::Other)
	{
		Flakes::WriteObject<Flakes::NetBinary::Type>(ExistingComponent, ComponentData.Flake);
		OnNodeComponentSourceEdited.Broadcast(ComponentData.NodeGuid, ExistingComponent, EventType);
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
		Args.NodeAndPinGuid.NodeGuid = IGraphNodeVisualizerInterface::Execute_GetNode_BP(Target);
	}
	else if (const IHeartGraphNodeInterface* NodeInterface = Cast<IHeartGraphNodeInterface>(Target))
	{
		Args.NodeAndPinGuid.NodeGuid = NodeInterface->GetNodeGuid();
	}
	else
	{
		if (Target->Implements<UGraphPinVisualizerInterface>())
		{
			Args.PinTarget = Target;
		}
		else if (const IHeartGraphPinInterface* PinInterface = Cast<IHeartGraphPinInterface>(Target))
		{
			Args.NodeAndPinGuid.NodeGuid = PinInterface->GetNodeGuid();
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

	ProxyGraph->GetOnNodeAddOrRemove().AddUObject(this, &ThisClass::OnNodeAddedOrRemoved_Proxy);
	ProxyGraph->GetOnNodeMoved().AddUObject(this, &ThisClass::OnNodesMoved_Proxy);
	ProxyGraph->GetOnNodeConnectionsChanged().AddUObject(this, &ThisClass::OnNodeConnectionsChanged_Proxy);
	ProxyGraph->GetOnExtensionAddOrRemove().AddUObject(this, &ThisClass::OnExtensionAddedOrRemoved_Proxy);
	ProxyGraph->GetOnNodeComponentAddOrRemove().AddUObject(this, &ThisClass::OnNodeComponentAddedOrRemoved_Proxy);

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

	// Add all existing extensions to the proxy graph
	for (auto&& Element : ReplicatedExtensions.Items)
	{
		UpdateExtensionProxy(Element, Heart::Net::Tags::Extension_Added);
	}

	// Add all existing node components to the proxy graph
	for (auto&& Element : ReplicatedNodeComponents.Items)
	{
		UpdateNodeComponentProxy(Element, Heart::Net::Tags::NodeComponent_Added);
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

	const FGameplayTagQuery Query = FGameplayTagQuery::BuildQuery
	(
		FGameplayTagQueryExpression()
			.AnyTagsMatch()
			.AddTag(RequestedEventType)
			.AddTag(Heart::Net::Tags::Permission_All)
	);

	return ClientPermissions.MatchesQuery(Query);
}

void UHeartGraphNetProxy::OnNodeAddedOrRemoved_Proxy(const FHeartNodeAddOrRemoveEvent& AddOrRemoveEvent)
{
	if (!IsValid(LocalClient))
	{
		return;
	}

	switch (AddOrRemoveEvent.Type)
	{
	case EHeartNodeAddOrRemoveEventType::Add:
		{
			if (RecursionGuards[NodeAdd]) return;

			UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnNodeAdded"))

			ensure(LocalClient->GetOwnerRole() == ROLE_AutonomousProxy);
			for (auto&& Node : AddOrRemoveEvent.Nodes)
			{
				auto&& GraphNode = ProxyGraph->GetNode(Node);

				FHeartReplicatedFlake NodeData;
				NodeData.Guid = Node;
				NodeData.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(GraphNode);
				UE_LOG(LogHeartNet, Log, TEXT("Sending node RPC data '%s' (%llu bytes)"),
					*GraphNode->GetName(), NodeData.Flake.Data.NumBytes());

				LocalClient->Server_OnNodeAdded(this, NodeData);
			}
		}
		break;
	case EHeartNodeAddOrRemoveEventType::Remove:
		{
			if (RecursionGuards[NodeDelete]) return;

			UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnNodeRemoved"))
			ensure(LocalClient->GetOwnerRole() == ROLE_AutonomousProxy);

			for (auto&& Node : AddOrRemoveEvent.Nodes)
			{
				LocalClient->Server_OnNodeRemoved(this, Node);
			}
		}
		break;
	}
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

		IHeartNodeLocationInterface* LocationInterface = ProxyGraph->GetNodeLocationInterface();

		FHeartNodeMoveEvent_Net Event;
		Algo::Transform(NodeMoveEvent.AffectedNodes, Event.AffectedNodes,
			[LocationInterface](const FHeartNodeGuid& Node)
			{
				FHeartReplicatedFlake NodeData;
				NodeData.Guid = Node;
				if (IHeartGraphInterface3D* Interface3D = Cast<IHeartGraphInterface3D>(LocationInterface))
				{
					NodeData.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(Interface3D->GetNodeLocation3D(Node), nullptr);
				}
				else
				{
					NodeData.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(LocationInterface->GetNodeLocation(Node), nullptr);
				}

				UE_LOG(LogHeartNet, Log, TEXT("Sending node RPC data '%s' (%llu bytes)"),
					*Node.ToString(), NodeData.Flake.Data.NumBytes());
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
					if (auto&& Connections = Node->ViewConnections(Pin);
						Connections.IsValid())
					{
						PinElement.PinConnections.Add(Pin, Connections.Get());
					}
					else
					{
						PinElement.PinConnections.Add(Pin,  FHeartGraphPinConnections{});
					}
				});

			NodeData.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(PinElement, nullptr);

			UE_LOG(LogHeartNet, Log, TEXT("Sending node RPC data '%s' (%llu bytes)"),
				*Node->GetName(), NodeData.Flake.Data.NumBytes());

			return NodeData;
		});

	LocalClient->Server_OnNodeConnectionsChanged(this, Event);
}

void UHeartGraphNetProxy::OnExtensionAddedOrRemoved_Proxy(UHeartGraphExtension* Extension, const Heart::Events::EComponentEventType Type)
{
	if (!IsValid(LocalClient))
	{
		return;
	}
	ensure(LocalClient->GetOwnerRole() == ROLE_AutonomousProxy);

	switch (Type)
	{
	case Heart::Events::EComponentEventType::Add:
		{
			if (RecursionGuards[ExtAdd]) return;

			UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnExtensionAdded"))

			{
				FHeartReplicatedFlake ExtensionData;
				ExtensionData.Guid = Extension->GetGuid();
				ExtensionData.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(Extension);
				UE_LOG(LogHeartNet, Log, TEXT("Sending extension RPC data '%s' (%llu bytes)"),
					*Extension->GetName(), ExtensionData.Flake.Data.NumBytes());

				LocalClient->Server_OnExtensionAdded(this, ExtensionData);
			}
		}
		break;
	case Heart::Events::EComponentEventType::Remove:
		{
			if (RecursionGuards[ExtDelete]) return;

			UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnExtensionRemoved"))

			{
				LocalClient->Server_OnExtensionRemoved(this, Extension->GetGuid());
			}
		}
		break;
	}
}

void UHeartGraphNetProxy::OnNodeComponentAddedOrRemoved_Proxy(const FHeartNodeGuid& Node,
	UHeartGraphNodeComponent* NodeComponent, const Heart::Events::EComponentEventType Type)
{
	if (!IsValid(LocalClient))
	{
		return;
	}
	ensure(LocalClient->GetOwnerRole() == ROLE_AutonomousProxy);

	switch (Type)
	{
	case Heart::Events::EComponentEventType::Add:
		{
			if (RecursionGuards[CompAdd]) return;

			UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnNodeComponentAdded"))

			{
				FHeartReplicatedNodeComponent NodeData;
				NodeData.NodeGuid = Node;
				NodeData.ComponentGuid = NodeComponent->GetGuid();
				NodeData.Flake = Flakes::MakeFlake<Flakes::NetBinary::Type>(NodeComponent);
				UE_LOG(LogHeartNet, Log, TEXT("Sending node component RPC data '%s' (%llu bytes)"),
					*NodeComponent->GetName(), NodeData.Flake.Data.NumBytes());

				LocalClient->Server_OnNodeComponentAdded(this, NodeData);
			}
		}
		break;
	case Heart::Events::EComponentEventType::Remove:
		{
			if (RecursionGuards[CompDelete]) return;

			UE_LOG(LogHeartNet, Log, TEXT("Proxy: OnNodeComponentRemoved"))

			{
				LocalClient->Server_OnNodeComponentRemoved(this, Node, NodeComponent->GetGuid());
			}
		}
		break;
	}
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

void UHeartGraphNetProxy::OnExtensionAdded_Client(const FHeartReplicatedFlake& ExtensionData)
{
	if (!CanClientPerformEvent(Heart::Net::Tags::Extension_Added))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to perform illegal event: '%s' on node '%s'"),
			*Heart::Net::Tags::Extension_Added.GetTag().ToString(),
			*ExtensionData.Guid.ToString())
		return;
	}

	EditReplicatedExtensionData(ExtensionData, Heart::Net::Tags::Extension_Added);
}

void UHeartGraphNetProxy::OnExtensionRemoved_Client(const FHeartExtensionGuid& Extension)
{
	if (!CanClientPerformEvent(Heart::Net::Tags::Extension_Removed))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to perform illegal event: '%s' on node '%s'"),
			*Heart::Net::Tags::Extension_Removed.GetTag().ToString(),
			*Extension.ToString())
		return;
	}

	if (IsValid(SourceGraph))
	{
		if (!SourceGraph->RemoveExtension(Extension))
		{
			UE_LOG(LogHeartNet, Warning, TEXT("Client attempt to perform remove extension failed: '%s'"),
				*Extension.ToString())
		}
	}
}

void UHeartGraphNetProxy::OnNodeComponentAdded_Client(const FHeartReplicatedNodeComponent& ComponentData)
{
	if (!CanClientPerformEvent(Heart::Net::Tags::NodeComponent_Added))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to perform illegal event: '%s' on node '%s'"),
			*Heart::Net::Tags::NodeComponent_Added.GetTag().ToString(),
			*ComponentData.NodeGuid.ToString())
		return;
	}

	EditReplicatedNodeComponentData(ComponentData, Heart::Net::Tags::NodeComponent_Added);
}

void UHeartGraphNetProxy::OnNodeComponentRemoved_Client(const FHeartNodeGuid& Node, const FHeartExtensionGuid& Component)
{
	if (!CanClientPerformEvent(Heart::Net::Tags::NodeComponent_Removed))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to perform illegal event: '%s' on node '%s'"),
			*Heart::Net::Tags::NodeComponent_Removed.GetTag().ToString(),
			*Node.ToString())
		return;
	}

	if (IsValid(SourceGraph))
	{
		const UHeartGraphNodeComponent* NodeComponent = SourceGraph->FindNodeComponentByGuid(Node, Component);

		if (!SourceGraph->RemoveNodeComponent(Node, NodeComponent->GetClass()))
		{
			UE_LOG(LogHeartNet, Warning, TEXT("Client attempt to perform remove node component failed: '%s'"),
				*Component.ToString())
		}
	}
}

void UHeartGraphNetProxy::ExecuteGraphAction_Client(const TSubclassOf<UHeartActionBase> Action, const FHeartRemoteGraphActionArguments& Args)
{
	if (!CanClientPerformEvent(Heart::Net::Tags::Permission_Actions))
	{
		UE_LOG(LogHeartNet, Warning, TEXT("Client attempted to execute graph action: '%s'"),
			*Heart::Net::Tags::Permission_Actions.GetTag().ToString())
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

	if (!IsValid(SourceGraph))
	{
		UE_LOG(LogHeartNet, Error, TEXT("Invalid Source Graph!"))
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

	if (!IsValid(SourceGraph))
	{
		UE_LOG(LogHeartNet, Error, TEXT("Invalid Source Graph!"))
		return;
	}

	Heart::Action::History::TryRedo(SourceGraph);
}

void UHeartGraphNetProxy::UpdateNodeProxy(const FHeartReplicatedFlake& Data, const FGameplayTag EventType)
{
	if (!IsValid(ProxyGraph))
	{
		UE_LOG(LogHeartNet, Error, TEXT("Invalid ProxyGraph!"))
		return;
	}

	if (UHeartGraphNode* ExistingNode = ProxyGraph->GetNode(Data.Guid.Get<FHeartNodeGuid>()))
	{
		Flakes::WriteObject<Flakes::NetBinary::Type>(ExistingNode, Data.Flake);
		OnNodeProxyUpdated.Broadcast(ExistingNode, EventType);
	}
	else
	{
		ensure(EventType == Heart::Net::Tags::Node_Added);

		UHeartGraphNode* NewNode = Flakes::CreateObject<UHeartGraphNode, Flakes::NetBinary::Type>(Data.Flake, ProxyGraph);
		if (!IsValid(NewNode))
		{
			UE_LOG(LogHeartNet, Error, TEXT("Failed to create Node from flake!"))
			return;
		}

		{
			// Prevent OnNodeAddedOrRemoved_Proxy from pinging this back to the server
			TGuardValue<bool> bRecursionGuard(RecursionGuards[NodeAdd], true);

			// @todo replace this with node data export/import
			PRAGMA_DISABLE_DEPRECATION_WARNINGS
			ProxyGraph->AddNode(NewNode);
			PRAGMA_ENABLE_DEPRECATION_WARNINGS
		}

		OnNodeProxyUpdated.Broadcast(NewNode, EventType);
	}
}

void UHeartGraphNetProxy::RemoveNodeProxy(const FHeartReplicatedFlake& Data)
{
	if (!IsValid(ProxyGraph))
	{
		UE_LOG(LogHeartNet, Error, TEXT("Invalid ProxyGraph!"))
		return;
	}

	const FHeartNodeGuid& Guid = Data.Guid.Get<FHeartNodeGuid>();

	// No Node to delete for some reason, maybe this is just a ping-back from a node we locally deleted.
	if (!ProxyGraph->GetNode(Guid))
	{
		return;
	}

	OnNodeProxyUpdated.Broadcast(ProxyGraph->GetNode(Guid), Heart::Net::Tags::Node_Removed);
	bool Result;
	{
		// Prevent OnNodeRemoved_Proxy from pinging this back to the server
		TGuardValue<bool> bRecursionGuard(RecursionGuards[NodeDelete], true);
		Result = ProxyGraph->RemoveNode(Guid);
	}
	if (!Result)
	{
		UE_LOG(LogHeartNet, Error, TEXT("Failed to remove node from ProxyGraph!"))
	}
}

void UHeartGraphNetProxy::UpdateExtensionProxy(const FHeartReplicatedFlake& Data, const FGameplayTag EventType)
{
	if (!IsValid(ProxyGraph))
	{
		UE_LOG(LogHeartNet, Error, TEXT("Invalid ProxyGraph!"))
		return;
	}

	if (UHeartGraphExtension* ExistingExtension = ProxyGraph->GetExtensionByGuid(Data.Guid.Get<FHeartExtensionGuid>()))
	{
		Flakes::WriteObject<Flakes::NetBinary::Type>(ExistingExtension, Data.Flake);
		OnExtensionProxyUpdated.Broadcast(ExistingExtension, EventType);
	}
	else
	{
		ensure(EventType == Heart::Net::Tags::Extension_Added);

		UHeartGraphExtension* NewExtension = Flakes::CreateObject<UHeartGraphExtension, Flakes::NetBinary::Type>(Data.Flake, ProxyGraph);
		if (!IsValid(NewExtension))
		{
			UE_LOG(LogHeartNet, Error, TEXT("Failed to create Extension from flake!"))
			return;
		}

		{
			// Prevent OnExtensionAdded_Proxy from pinging this back to the server
			TGuardValue<bool> bRecursionGuard(RecursionGuards[ExtAdd], true);
			ProxyGraph->AddExtensionInstance(NewExtension);
		}

		OnExtensionProxyUpdated.Broadcast(NewExtension, EventType);
	}
}

void UHeartGraphNetProxy::RemoveExtensionProxy(const FHeartReplicatedFlake& Data)
{
	if (!IsValid(ProxyGraph))
	{
		UE_LOG(LogHeartNet, Error, TEXT("Invalid ProxyGraph!"))
		return;
	}

	const FHeartExtensionGuid& Guid = Data.Guid.Get<FHeartExtensionGuid>();

	OnExtensionProxyUpdated.Broadcast(ProxyGraph->GetExtensionByGuid(Guid), Heart::Net::Tags::Extension_Removed);
	bool Result;
	{
		// Prevent OnExtensionRemoved_Proxy from pinging this back to the server
		TGuardValue<bool> bRecursionGuard(RecursionGuards[ExtDelete], true);
		Result = ProxyGraph->RemoveExtension(Guid);
	}

	if (!Result)
	{
		UE_LOG(LogHeartNet, Error, TEXT("Failed to remove node extension from ProxyGraph!"))
	}
}

void UHeartGraphNetProxy::UpdateNodeComponentProxy(const FHeartReplicatedNodeComponent& Data, const FGameplayTag EventType)
{
	if (!IsValid(ProxyGraph))
	{
		UE_LOG(LogHeartNet, Error, TEXT("Invalid ProxyGraph!"))
		return;
	}

	UClass* ComponentClass = Cast<UClass>(Data.Flake.Struct.TryLoad());

	UHeartGraphNodeComponent* NodeComponent;
	{
		// If the FindOrAdd call creates this component we don't want it triggering OnNodeComponentAddedOrRemoved_Proxy
		TGuardValue<bool> bRecursionGuard(RecursionGuards[CompAdd], true);
		NodeComponent = ProxyGraph->FindOrAddNodeComponent(Data.NodeGuid, ComponentClass);
	}

	if (!IsValid(NodeComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to update proxy node component! Failed to create node component."))
		return;
	}

#if DO_ENSURE
	// If the guids didn't match, then this is a newly sent component, verify the server is adding this component correctly.
	if (NodeComponent->GetGuid() != Data.ComponentGuid)
	{
		ensure(EventType == Heart::Net::Tags::NodeComponent_Added);
	}
#endif

	// Write the Flake into the component, and broadcast update.
	Flakes::WriteObject<Flakes::NetBinary::Type>(NodeComponent, Data.Flake);
	OnNodeComponentProxyUpdated.Broadcast(Data.NodeGuid, NodeComponent, EventType);
}

void UHeartGraphNetProxy::RemoveNodeComponentProxy(const FHeartReplicatedNodeComponent& Data)
{
	if (!IsValid(ProxyGraph))
	{
		UE_LOG(LogHeartNet, Error, TEXT("Invalid ProxyGraph!"))
		return;
	}

	UClass* ComponentClass = Cast<UClass>(Data.Flake.Struct.TryLoad());

	FHeartNodeGuid Node;
	UHeartGraphNodeComponent* Component = ProxyGraph->GetNodeComponent(Data.NodeGuid, ComponentClass);
	ProxyGraph->FindNodeComponentByGuid(ComponentClass, Data.ComponentGuid, Node, Component);
	OnNodeComponentProxyUpdated.Broadcast(Node, Component, Heart::Net::Tags::NodeComponent_Removed);
	bool Result;
	{
		// Prevent OnNodeComponentAddedOrRemoved_Proxy from pinging this back to the server
		TGuardValue<bool> bRecursionGuard(RecursionGuards[CompDelete], true);
		Result = ProxyGraph->RemoveNodeComponent(Node, Component->GetClass());
	}

	if (!Result)
	{
		UE_LOG(LogHeartNet, Error, TEXT("Failed to remove node component from ProxyGraph!"))
	}
}

void UHeartGraphNetProxy::PostReplicatedAdd(const FHeartReplicatedData& Array, const FHeartReplicatedFlake& Data)
{
	if (&Array == &ReplicatedNodes)
	{
		UpdateNodeProxy(Data, Heart::Net::Tags::Node_Added);
	}
	else if (&Array == &ReplicatedExtensions)
	{
		UpdateExtensionProxy(Data, Heart::Net::Tags::Extension_Added);
	}
}

void UHeartGraphNetProxy::PostReplicatedChange(const FHeartReplicatedData& Array, const FHeartReplicatedFlake& Data)
{
	if (&Array == &ReplicatedNodes)
	{
		// @todo clients have no idea what kind of changes the server is making...
		UpdateNodeProxy(Data, Heart::Net::Tags::Other);
	}
	else if (&Array == &ReplicatedExtensions)
	{
		// @todo clients have no idea what kind of changes the server is making...
		UpdateExtensionProxy(Data, Heart::Net::Tags::Other);
	}
}

void UHeartGraphNetProxy::PreReplicatedRemove(const FHeartReplicatedData& Array, const FHeartReplicatedFlake& Data)
{
	if (&Array == &ReplicatedNodes)
	{
		RemoveNodeProxy(Data);
	}
	else if (&Array == &ReplicatedExtensions)
	{
		RemoveExtensionProxy(Data);
	}
}

void UHeartGraphNetProxy::PostReplicatedAdd_NodeComponent(const FHeartReplicatedNodeComponent& Data)
{
	UpdateNodeComponentProxy(Data, Heart::Net::Tags::NodeComponent_Added);
}

void UHeartGraphNetProxy::PostReplicatedChange_NodeComponent(const FHeartReplicatedNodeComponent& Data)
{
	// @todo clients have no idea what kind of changes the server is making...
	UpdateNodeComponentProxy(Data, Heart::Net::Tags::Other);
}

void UHeartGraphNetProxy::PreReplicatedRemove_NodeComponent(const FHeartReplicatedNodeComponent& Data)
{
	RemoveNodeComponentProxy(Data);
}
