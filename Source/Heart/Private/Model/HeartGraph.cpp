// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraph.h"
#include "Model/HeartGraphExtension.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartNodeEdit.h"
#include "ModelView/HeartGraphSchema.h"

#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraph)

#define LOCTEXT_NAMESPACE "HeartGraph"

DEFINE_LOG_CATEGORY(LogHeartGraph)

UHeartGraph::UHeartGraph()
{
#if WITH_EDITORONLY_DATA
	if (auto SparseGraphData = GetHeartGraphSparseClassData();
		SparseGraphData->GraphTypeName.IsEmpty())
	{
		SparseGraphData->GraphTypeName = LOCTEXT("DefaultGraphTypeName", "Heart");
	}
#endif
}

#if WITH_EDITOR
void UHeartGraph::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	// Save the EdGraph with us in the editor
	UHeartGraph* This = CastChecked<UHeartGraph>(InThis);
	Collector.AddReferencedObject(This->HeartEdGraph, This);

	Super::AddReferencedObjects(InThis, Collector);
}
#endif

UWorld* UHeartGraph::GetWorld() const
{
	if (!IsTemplate())
	{
#if WITH_EDITOR
		// @todo hack, fix this
		static bool GetWorldRecursion = false;
		if (GetWorldRecursion) return nullptr;
		TGuardValue<bool> RecursionGuard(GetWorldRecursion, true);
#endif

		UWorld* World = nullptr;
		if (GetSchema()->TryGetWorldForGraph(this, World))
		{
			return World;
		}
	}

	return Super::GetWorld();
}

void UHeartGraph::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

#if WITH_EDITOR
	if (IsTemplate())
	{
		return;
	}
#endif

	GetSchema()->OnPreSaveGraph(this, SaveContext);

#if WITH_EDITOR
	if (SaveContext.IsCooking())
	{
		if (GetSchema()->FlushNodesForRuntime)
		{
			Nodes.Empty();
		}
	}
#endif
}

void UHeartGraph::PostInitProperties()
{
	Super::PostInitProperties();
	if (!Guid.IsValid())
	{
		Guid = FHeartGraphGuid::New();
	}
}

void UHeartGraph::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	// Clean asset in Editor, during loading

	for (auto It = Extensions.CreateIterator(); It; ++It)
	{
		auto&& Extension = *It;
		if (!Extension.Key.IsValid() ||
			!IsValid(Extension.Value))
		{
			It.RemoveCurrent();
		}
	}

	TArray<FHeartNodeGuid> CleanedUpNodes;

	for (auto It = Nodes.CreateIterator(); It; ++It)
	{
		auto&& Node = *It;

		if (!IsValid(Node.Value))
		{
			CleanedUpNodes.Add(Node.Key);
			It.RemoveCurrent();
			continue;
		}

		UObject* NodeObject = Node.Value->GetNodeObject();
		if (!IsValid(NodeObject))
		{
			CleanedUpNodes.Add(Node.Key);
			It.RemoveCurrent();
			continue;
		}

		if (NodeObject->GetOuter() == this)
		{
			NodeObject->Rename(nullptr, Node.Value);
		}
	}

	for (auto It = NodeComponents.CreateIterator(); It; ++It)
	{
		auto& NodeMap = *It;
		if (!IsValid(NodeMap.Key))
		{
			It.RemoveCurrent();
			continue;
		}

		for (auto SubIt = NodeMap.Value.Components.CreateIterator(); It; ++It)
		{
			auto&& Component = *SubIt;
			if (!Component.Key.IsValid() ||
				!IsValid(Component.Value) ||
				CleanedUpNodes.Contains(Component.Key))
			{
				It.RemoveCurrent();
			}
		}
	}

	// Cannot do this without also running the presave action
	/*
	if (!IsTemplate())
	{
		GetSchema()->RefreshGraphExtensions(this);
	}
	*/
#endif
}

void UHeartGraph::PostDuplicate(const EDuplicateMode::Type DuplicateMode)
{
#if WITH_EDITOR
	// The HeartEdGraph doesn't need to persist for graphs duplicated during gameplay
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		HeartEdGraph = nullptr;
	}

	if (DuplicateMode == EDuplicateMode::PIE)
	{
		if (GetSchema()->FlushNodesForRuntime)
		{
			Nodes.Empty();
		}
	}
#endif

	Super::PostDuplicate(DuplicateMode);
}

void UHeartGraph::NotifyNodeLocationChanged(const FHeartNodeGuid& AffectedNode, const bool InProgress)
{
	if (!AffectedNode.IsValid()) return;

	FHeartNodeMoveEvent Event;
	Event.AffectedNodes.Add(GetNode(AffectedNode));
	Event.MoveFinished = !InProgress;
	HandleNodeMoveEvent(Event);
}

void UHeartGraph::NotifyNodeLocationsChanged(const TSet<FHeartNodeGuid>& AffectedNodes, const bool InProgress)
{
	if (AffectedNodes.IsEmpty()) return;

	FHeartNodeMoveEvent Event;
	for (auto&& Element : AffectedNodes)
	{
		if (Element.IsValid())
		{
			if (UHeartGraphNode* Node = GetNode(Element);
				ensure(IsValid(Node)))
			{
				Event.AffectedNodes.Add(Node);
			}
		}
	}
	Event.MoveFinished = !InProgress;
	HandleNodeMoveEvent(Event);
}

void UHeartGraph::ForEachNode(const TFunctionRef<bool(UHeartGraphNode*)>& Iter) const
{
	for (auto&& Element : Nodes)
	{
		if (ensure(Element.Value))
		{
			if (!Iter(Element.Value))
			{
				break;
			}
		}
	}
}

void UHeartGraph::ForEachExtension(const TFunctionRef<bool(UHeartGraphExtension*)>& Iter) const
{
	for (auto&& Element : Extensions)
	{
		if (ensure(Element.Value))
		{
			if (Iter(Element.Value)) return;
		}
	}
}

void UHeartGraph::HandleNodeAddEvent(const FHeartNodeAddEvent& Event)
{
	// @todo batch this as well?
	for (const FHeartNodeGuid& NodeGuid : Event.NewNodes)
	{
		if (UHeartGraphNode* Node = GetNode(NodeGuid);
			ensure(IsValid(Node)))
		{
			OnNodeAdded.Broadcast(Node);
		}
	}
}

void UHeartGraph::HandleNodeRemoveEvent(const FHeartNodeRemoveEvent& Event)
{
	// @todo batch this as well?
	for (UHeartGraphNode* Node : Event.AffectedNodes)
	{
		OnNodeRemoved.Broadcast(Node);
	}
}

void UHeartGraph::HandleNodeMoveEvent(const FHeartNodeMoveEvent& Event)
{
	OnNodeMoved.Broadcast(Event);
}

void UHeartGraph::HandleGraphConnectionEvent(const FHeartGraphConnectionEvent& Event)
{
	{
#if WITH_EDITOR
		FEditorScriptExecutionGuard ScriptExecutionGuard;
#endif
		BP_OnNodeConnectionsChanged(Event);
	}
	OnNodeConnectionsChanged.Broadcast(Event);
}

UHeartGraphNode* UHeartGraph::GetNode(const FHeartNodeGuid& NodeGuid) const
{
	auto&& Result = Nodes.Find(NodeGuid);
	return Result ? *Result : nullptr;
}

void UHeartGraph::GetNodeGuids(TArray<FHeartNodeGuid>& OutGuids) const
{
	Nodes.GenerateKeyArray(OutGuids);
}

void UHeartGraph::GetNodeArray(TArray<UHeartGraphNode*>& OutNodes) const
{
	Nodes.GenerateValueArray(ObjectPtrWrap(OutNodes));
}

Heart::Query::FGraphNodeQuery UHeartGraph::QueryNodes() const
{
	return Heart::Query::FGraphNodeQuery(this);
}

TSubclassOf<UHeartGraphSchema> UHeartGraph::GetSchemaClass_Implementation() const
{
	return SchemaClass;
}

const UHeartGraphSchema* UHeartGraph::GetSchema() const
{
	// New path
	if (IsValid(SchemaClass))
	{
		return SchemaClass.GetDefaultObject();
	}

	// Old path
	UClass* Class;
	{
#if WITH_EDITOR
		// GetSchemaClass is a BlueprintNativeEvent, but we should be able to call it in the editor.
		FEditorScriptExecutionGuard ScriptExecutionGuard;
#endif
		Class = GetSchemaClass();
	}

	if (!ensure(IsValid(Class)))
	{
		UE_LOG(LogHeartGraph, Warning, TEXT("GetSchemaClass for Graph '%s' returned nullptr!"), *GetName())
		return GetDefault<UHeartGraphSchema>();
	}

	return GetDefault<UHeartGraphSchema>(Class);
}

const UHeartGraphSchema* UHeartGraph::GetSchemaTyped_K2(TSubclassOf<UHeartGraphSchema>) const
{
	return GetSchema();
}

UHeartGraphExtension* UHeartGraph::GetExtensionByGuid(const FHeartExtensionGuid ExtensionGuid,
													  TSubclassOf<UHeartGraphExtension>) const
{
	if (auto Extension = Extensions.Find(ExtensionGuid))
	{
		if (Extension && IsValid(*Extension))
		{
			return *Extension;
		}
	}

	return nullptr;
}

UHeartGraphExtension* UHeartGraph::GetExtension(const TSubclassOf<UHeartGraphExtension> Class) const
{
	if (!IsValid(Class) ||
		Class == UHeartGraphExtension::StaticClass())
	{
		return nullptr;
	}

	for (auto&& Extension : Extensions)
	{
		if (Extension.Value->GetClass()->IsChildOf(Class))
		{
			return Extension.Value;
		}
	}

	return nullptr;
}

TArray<UHeartGraphExtension*> UHeartGraph::GetAllExtensions() const
{
	TArray<UHeartGraphExtension*> Out;
	Extensions.GenerateValueArray(ObjectPtrWrap(Out));
	return Out;
}

TArray<UHeartGraphExtension*> UHeartGraph::GetExtensionsOfClass(const TSubclassOf<UHeartGraphExtension> Class) const
{
	TArray<UHeartGraphExtension*> Out;

	for (auto&& Extension : Extensions)
	{
		if (Extension.Value->GetClass()->IsChildOf(Class))
		{
			Out.Add(Extension.Value);
		}
	}

	return Out;
}

UHeartGraphExtension* UHeartGraph::AddExtension(const TSubclassOf<UHeartGraphExtension> Class)
{
	if (!IsValid(Class))
	{
		return nullptr;
	}

	UHeartGraphExtension* NewExtension = NewObject<UHeartGraphExtension>(this, Class);
	NewExtension->Guid = FHeartExtensionGuid::New(); // @todo isn't this redundant, and gets assigned in PostInitProperties?
	Extensions.Add(NewExtension->Guid, NewExtension);
	NewExtension->PostComponentAdded();

	OnExtensionAdded.Broadcast(NewExtension);

	return NewExtension;
}

bool UHeartGraph::AddExtensionInstance(UHeartGraphExtension* Extension)
{
	if (IsValid(Extension))
	{
		// Should only add instances that have been created/renamed to us.
		check(Extension->GetOuter() == this);
		check(!Extensions.Contains(Extension->Guid));

		Extensions.Add(Extension->Guid, Extension);
		Extension->PostComponentAdded();

		OnExtensionAdded.Broadcast(Extension);
	}

	return false;
}

bool UHeartGraph::RemoveExtension(const FHeartExtensionGuid ExtensionGuid)
{
	if (!Extensions.Contains(ExtensionGuid))
	{
		return false;
	}

	auto&& Extension = Extensions[ExtensionGuid];
	Extension->PreComponentRemoved();
	Extensions.Remove(ExtensionGuid);
	OnExtensionRemoved.Broadcast(Extension);
	return true;
}

void UHeartGraph::RemoveExtensionsByClass(const TSubclassOf<UHeartGraphExtension> Class)
{
	for (auto ExtensionIt = Extensions.CreateIterator(); ExtensionIt; ++ExtensionIt)
	{
		auto&& Extension = ExtensionIt.Value();
		if (!IsValid(Extension))
		{
			ExtensionIt.RemoveCurrent();
			continue;
		}

		if (Extension.GetClass()->IsChildOf(Class))
		{
			Extension->PreComponentRemoved();
			ExtensionIt.RemoveCurrent();
			OnExtensionRemoved.Broadcast(Extension);
		}
	}
}

UHeartGraphNode* UHeartGraph::CreateNode_Instanced(const TSubclassOf<UHeartGraphNode> GraphNodeClass,
												   const UClass* NodeObjectClass, const FVector2D& Location)
{
	if (!ensure(IsValid(GraphNodeClass) &&
				IsValid(NodeObjectClass)))
	{
		return nullptr;
	}

	return Heart::API::FNodeCreator::CreateNode_Instanced(this, GraphNodeClass, NodeObjectClass, Location);
}

UHeartGraphNode* UHeartGraph::CreateNode_Reference(const TSubclassOf<UHeartGraphNode> GraphNodeClass,
												   const UObject* NodeObject, const FVector2D& Location)
{
	if (!ensure(IsValid(GraphNodeClass) &&
				IsValid(NodeObject)))
	{
		return nullptr;
	}

	return Heart::API::FNodeCreator::CreateNode_Reference(this, GraphNodeClass, NodeObject, Location);
}

void UHeartGraph::AddNode(UHeartGraphNode* Node)
{
	checkSlow(Node->GetOuter() == this);

	if (!ensure(IsValid(Node) && Node->GetGuid().IsValid()))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("Tried to add invalid node!"))
		return;
	}

	// @todo uncomment this and if something is tripping it, solve that!
	/*
	if (!ensure(IsValid(Node->GetNodeObject())))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("Tried to add a node with invalid object!"))
		return;
	}
	*/

	const FHeartNodeGuid& NodeGuid = Node->GetGuid();

	if (!ensure(!Nodes.Contains(NodeGuid)))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("Tried to add node already in graph!"))
		return;
	}

	Nodes.Add(NodeGuid, Node);
	FHeartNodeAddEvent Event;
	Event.NewNodes.Add(NodeGuid);
	HandleNodeAddEvent(Event);
}

bool UHeartGraph::RemoveNode(const FHeartNodeGuid& NodeGuid)
{
	return Heart::API::FNodeEdit::DeleteNode(this, NodeGuid);
}

UHeartGraphNodeComponent* UHeartGraph::GetNodeComponent(const FHeartNodeGuid& Node, const TSubclassOf<UHeartGraphNodeComponent> Class) const
{
	if (!Node.IsValid() ||
		!IsValid(Class) ||
		Class == UHeartGraphNodeComponent::StaticClass())
	{
		return nullptr;
	}

	if (const FHeartGraphNodeComponentMap* ClassMap = NodeComponents.Find(Class))
	{
		return ClassMap->Find(Node);
	}

	return nullptr;
}

TArray<UHeartGraphNodeComponent*> UHeartGraph::GetNodeComponentsForNode(const FHeartNodeGuid& Node) const
{
	if (!Node.IsValid())
	{
		return {};
	}

	TArray<UHeartGraphNodeComponent*> Out;

	for (auto&& ClassMap : NodeComponents)
	{
		for (auto&& NodeComponent : ClassMap.Value.Components)
		{
			if (NodeComponent.Key == Node)
			{
				Out.Add(NodeComponent.Value);
			}
		}
	}

	return Out;
}

TArray<UHeartGraphNodeComponent*> UHeartGraph::GetNodeComponentsOfClass(const TSubclassOf<UHeartGraphNodeComponent> Class) const
{
	TArray<UHeartGraphNodeComponent*> Out;

	if (const FHeartGraphNodeComponentMap* NodeMap = NodeComponents.Find(Class))
	{
		NodeMap->Components.GenerateValueArray(ObjectPtrWrap(Out));
	}

	return Out;
}

UHeartGraphNodeComponent* UHeartGraph::FindOrAddNodeComponent(const FHeartNodeGuid& Node, const TSubclassOf<UHeartGraphNodeComponent> Class)
{
	if (!Node.IsValid() ||
		!IsValid(Class) ||
		Class == UHeartGraphNodeComponent::StaticClass())
	{
		return nullptr;
	}

	FHeartGraphNodeComponentMap& NodeMap = NodeComponents.FindOrAdd(Class);

	// Look for an existing component for the node first.
	if (auto&& ExistingComponent = NodeMap.Find(Node))
	{
		return ExistingComponent;
	}

	// Create and assign a new component for the node.
	UHeartGraphNodeComponent* NewComponent =
		NodeMap.Components.Add(Node, NewObject<UHeartGraphNodeComponent>(this, Class));

	// @todo isn't this redundant, and gets assigned in PostInitProperties?
	NewComponent->Guid = FHeartExtensionGuid::New();

	NewComponent->PostComponentAdded();

	OnComponentAdded.Broadcast(Node, NewComponent);

	return NewComponent;
}

bool UHeartGraph::RemoveNodeComponent(const FHeartNodeGuid& Node, const TSubclassOf<UHeartGraphNodeComponent> Class)
{
	if (!Node.IsValid() ||
		!IsValid(Class) ||
		Class == UHeartGraphNodeComponent::StaticClass())
	{
		return false;
	}

	if (FHeartGraphNodeComponentMap* NodeMap = NodeComponents.Find(Class))
	{
		TObjectPtr<UHeartGraphNodeComponent> Component = nullptr;
		NodeMap->Components.RemoveAndCopyValue(Node, Component);
		if (IsValid(Component))
		{
			Component->PreComponentRemoved();
			OnComponentRemoved.Broadcast(Node, Component);
			return true;
		}
	}

	return false;
}

void UHeartGraph::RemoveComponentsForNode(const FHeartNodeGuid& Node)
{
	for (auto&& NodeMap : NodeComponents)
	{
		NodeMap.Value.Components.Remove(Node);
	}
}

void UHeartGraph::RemoveComponentsForNodes(const TConstArrayView<FHeartNodeGuid> InNodes)
{
	for (auto&& NodeMap : NodeComponents)
	{
		for (auto&& Node : InNodes)
		{
			NodeMap.Value.Components.Remove(Node);
		}
	}
}

Heart::API::FPinEdit UHeartGraph::EditConnections()
{
	return Heart::API::FPinEdit(this);
}

bool UHeartGraph::ConnectPins(const FHeartGraphPinReference& PinA, const FHeartGraphPinReference& PinB)
{
	return EditConnections().Connect(PinA, PinB).Modified();
}

bool UHeartGraph::DisconnectPins(const FHeartGraphPinReference& PinA, const FHeartGraphPinReference& PinB)
{
	return EditConnections().Disconnect(PinA, PinB).Modified();
}

bool UHeartGraph::DisconnectAllPins(const FHeartGraphPinReference& Pin)
{
	return EditConnections().DisconnectAll(Pin).Modified();
}

#undef LOCTEXT_NAMESPACE


	/*----------------------------
			DEPRECATED API
	----------------------------*/

#include "GraphRegistry/HeartGraphNodeRegistry.h"

UHeartGraphNode* UHeartGraph::CreateNodeFromClass(const UClass* NodeClass, const FVector2D& Location)
{
	if (!ensure(IsValid(NodeClass)))
	{
		return nullptr;
	}

	TSubclassOf<UHeartGraphNode> GraphNodeClass;

	if (auto&& RegistrySubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
				GraphNodeClass = RegistrySubsystem->GetRegistry(GetClass())->GetGraphNodeClassForNode(FHeartNodeSource(NodeClass));
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	}

	if (!IsValid(GraphNodeClass))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("GetGraphNodeClassForNode returned nullptr when trying to spawn node of class '%s'!"), *NodeClass->GetName())
		return nullptr;
	}

	return Heart::API::FNodeCreator::CreateNode_Instanced(this, GraphNodeClass, NodeClass, Location);
}

UHeartGraphNode* UHeartGraph::CreateNodeFromObject(UObject* NodeObject, const FVector2D& Location)
{
	if (!ensure(IsValid(NodeObject)))
	{
		return nullptr;
	}

	TSubclassOf<UHeartGraphNode> GraphNodeClass;

	if (auto&& RegistrySubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
				GraphNodeClass = RegistrySubsystem->GetRegistry(GetClass())->GetGraphNodeClassForNode(FHeartNodeSource(NodeObject));
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	}

	return Heart::API::FNodeCreator::CreateNode_Reference(this, GraphNodeClass, NodeObject, Location);
}

void UHeartGraph::NotifyNodeLocationsChanged(const TSet<UHeartGraphNode*>& AffectedNodes, const bool InProgress)
{
	FHeartNodeMoveEvent Event;
	Event.AffectedNodes = AffectedNodes;
	Event.MoveFinished = !InProgress;
	HandleNodeMoveEvent(Event);
}

void UHeartGraph::NotifyNodeConnectionsChanged(const FHeartGraphConnectionEvent& Event)
{
	HandleGraphConnectionEvent(Event);
}