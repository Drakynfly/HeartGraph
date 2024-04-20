// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraph.h"
#include "Model/HeartGraphExtension.h"
#include "Model/HeartGraphNode.h"
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

	if (!IsTemplate())
	{
		GetSchema()->RefreshGraphExtensions(this);
	}
}

void UHeartGraph::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	TArray<FHeartNodeGuid> DeadNodes;

	// Fix-up node map in Editor, when loading asset
	for (auto&& Node : Nodes)
	{
		if (!IsValid(Node.Value))
		{
			DeadNodes.Add(Node.Key);
			continue;
		}

		if (Node.Value->NodeObject->GetOuter() == this)
		{
			Node.Value->NodeObject->Rename(nullptr, Node.Value);
		}
	}

	for (const FHeartNodeGuid& DeadNode : DeadNodes)
	{
		Nodes.Remove(DeadNode);
	}
#endif
}

void UHeartGraph::PostDuplicate(const EDuplicateMode::Type DuplicateMode)
{
#if WITH_EDITOR
	// The HeartEdGraph doesn't need to persist for graphs duplicated during gameplay
	if (GetWorld()->IsGameWorld())
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

void UHeartGraph::NotifyNodeLocationsChanged(const TSet<UHeartGraphNode*>& AffectedNodes, const bool InProgress)
{
	FHeartNodeMoveEvent Event;
	Event.AffectedNodes = AffectedNodes;
	Event.MoveFinished = !InProgress;
	OnNodeMoved.Broadcast(Event);
}

void UHeartGraph::NotifyNodeConnectionsChanged(const FHeartGraphConnectionEvent& Event)
{
	{
#if WITH_EDITOR
		FEditorScriptExecutionGuard ScriptExecutionGuard;
#endif
		BP_OnNodeConnectionsChanged(Event);
	}
	OnNodeConnectionsChanged.Broadcast(Event);
}

UHeartGraph* UHeartGraph::GetHeartGraph() const
{
	return const_cast<ThisClass*>(this);
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

TSubclassOf<UHeartGraphSchema> UHeartGraph::GetSchemaClass_Implementation() const
{
	return UHeartGraphSchema::StaticClass();
}

const UHeartGraphSchema* UHeartGraph::GetSchema() const
{
	return UHeartGraphSchema::Get(GetClass());
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
	for (auto&& Extension : Extensions)
	{
		if (Extension.Value->GetClass()->IsChildOf(Class))
		{
			return Extension.Value;
		}
	}

	return nullptr;
}

TArray<UHeartGraphExtension*> UHeartGraph::GetExtensions(const TSubclassOf<UHeartGraphExtension> Class) const
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
	NewExtension->Guid = FHeartExtensionGuid::New();
	Extensions.Add(NewExtension->Guid, NewExtension);
	NewExtension->PostExtensionAdded();

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
		Extension->PostExtensionAdded();

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
	Extension->PreExtensionRemove();
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
			Extension->PreExtensionRemove();
			ExtensionIt.RemoveCurrent();
			OnExtensionRemoved.Broadcast(Extension);
		}
	}
}

UHeartGraphNode* UHeartGraph::Internal_CreateNode_Instanced(const TSubclassOf<UHeartGraphNode>& GraphNodeClass, const UClass* NodeObjectClass, const FVector2D& Location)
{
	checkSlow(IsValid(GraphNodeClass));
	checkSlow(IsValid(NodeObject));

	UHeartGraphNode* NewGraphNode = NewObject<UHeartGraphNode>(this, GraphNodeClass);
	NewGraphNode->Guid = FHeartNodeGuid::New();
	NewGraphNode->NodeObject = NewObject<UObject>(NewGraphNode, NodeObjectClass);
	NewGraphNode->Location = Location;

	NewGraphNode->OnCreate();

	return NewGraphNode;
}

UHeartGraphNode* UHeartGraph::Internal_CreateNode_Reference(const TSubclassOf<UHeartGraphNode>& GraphNodeClass, const UObject* NodeObject, const FVector2D& Location)
{
	checkSlow(IsValid(GraphNodeClass));
	checkSlow(IsValid(NodeObject));

	auto&& NewGraphNode = NewObject<UHeartGraphNode>(this, GraphNodeClass);
	NewGraphNode->Guid = FHeartNodeGuid::New();
	NewGraphNode->NodeObject = const_cast<UObject*>(NodeObject); // @todo temp const_cast in lieu of proper const safety enforcement
	NewGraphNode->Location = Location;

	NewGraphNode->OnCreate();

	return NewGraphNode;
}

UHeartGraphNode* UHeartGraph::CreateNode_Instanced(const TSubclassOf<UHeartGraphNode> GraphNodeClass,
												   const UClass* NodeObjectClass, const FVector2D& Location)
{
	if (!ensure(IsValid(GraphNodeClass) &&
				IsValid(NodeObjectClass)))
	{
		return nullptr;
	}

	return Internal_CreateNode_Instanced(GraphNodeClass, NodeObjectClass, Location);
}

UHeartGraphNode* UHeartGraph::CreateNode_Reference(const TSubclassOf<UHeartGraphNode> GraphNodeClass,
												   const UObject* NodeObject, const FVector2D& Location)
{
	if (!ensure(IsValid(GraphNodeClass) &&
				IsValid(NodeObject)))
	{
		return nullptr;
	}

	return Internal_CreateNode_Reference(GraphNodeClass, NodeObject, Location);
}

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
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	}

	if (!IsValid(GraphNodeClass))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("GetGraphNodeClassForNode returned nullptr when trying to spawn node of class '%s'!"), *NodeClass->GetName())
		return nullptr;
	}

	return Internal_CreateNode_Instanced(GraphNodeClass, NodeClass, Location);
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
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	}

	return CreateNode_Reference(GraphNodeClass, NodeObject, Location);
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

	const FHeartNodeGuid NodeGuid = Node->GetGuid();

	if (!ensure(!Nodes.Contains(NodeGuid)))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("Tried to add node already in graph!"))
		return;
	}

	Nodes.Add(NodeGuid, Node);

	OnNodeAdded.Broadcast(Node);
}

bool UHeartGraph::RemoveNode(const FHeartNodeGuid& NodeGuid)
{
	if (!ensure(NodeGuid.IsValid()))
	{
		return false;
	}

	// Remove all connections that will be orphaned by removing this node
	EditConnections().DisconnectAll(NodeGuid);

	auto&& NodeBeingRemoved = Nodes.Find(NodeGuid);
	const int32 Removed = Nodes.Remove(NodeGuid);

	if (const TObjectPtr<UHeartGraphNode> Node = NodeBeingRemoved ? *NodeBeingRemoved : nullptr;
		IsValid(Node))
	{
		OnNodeRemoved.Broadcast(Node);
	}

	return !!Removed;
}

Heart::Connections::FEdit UHeartGraph::EditConnections()
{
	return Heart::Connections::FEdit(this);
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