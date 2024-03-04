// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraph.h"

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
	GetHeartGraphSparseClassData()->GraphTypeName = LOCTEXT("DefaultGraphTypeName", "Heart");
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

void UHeartGraph::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	TArray<FHeartNodeGuid> DeadNodes;

	// Fix-up node map in editor, when loading asset
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

void UHeartGraph::NotifyNodeConnectionsChanged(const TSet<UHeartGraphNode*>& AffectedNodes, const TSet<FHeartPinGuid>& AffectedPins)
{
	FHeartGraphConnectionEvent Event;
	Event.AffectedNodes = AffectedNodes;
	Event.AffectedPins = AffectedPins;
	NotifyNodeConnectionsChanged(Event);
}

void UHeartGraph::NotifyNodeConnectionsChanged(const FHeartGraphConnectionEvent& Event)
{
	BP_OnNodeConnectionsChanged(Event);
	OnNodeConnectionsChanged.Broadcast(Event);
}

UHeartGraph* UHeartGraph::GetHeartGraph() const
{
	return const_cast<UHeartGraph*>(this);
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
	// *le sign* epic templates mess this up . . .
	TArray<TObjectPtr<UHeartGraphNode>> NodeArray;
	Nodes.GenerateValueArray(NodeArray);
	OutNodes = NodeArray;
}

TSubclassOf<UHeartGraphSchema> UHeartGraph::GetSchemaClass_Implementation() const
{
	return UHeartGraphSchema::StaticClass();
}

const UHeartGraphSchema* UHeartGraph::GetSchema() const
{
	return UHeartGraphSchema::Get(GetClass());
}

const UHeartGraphSchema* UHeartGraph::GetSchemaTyped_K2(TSubclassOf<UHeartGraphSchema> Class) const
{
	return GetSchema();
}

UHeartGraphExtension* UHeartGraph::GetExtension(const TSubclassOf<UHeartGraphExtension> Class) const
{
	for (auto&& Extension : Extensions)
	{
		if (Extension.Key->IsChildOf(Class))
		{
			return Extension.Value;
		}
	}

	return nullptr;
}

UHeartGraphExtension* UHeartGraph::AddExtension(const TSubclassOf<UHeartGraphExtension> Class)
{
	if (!IsValid(Class))
	{
		return nullptr;
	}

	if (Extensions.Contains(Class))
	{
		return nullptr;
	}

	UHeartGraphExtension* NewExtension = NewObject<UHeartGraphExtension>(this, Class);
	Extensions.Add(Class, NewExtension);
	NewExtension->PostExtensionAdded();
	return NewExtension;
}

bool UHeartGraph::AddExtensionInstance(UHeartGraphExtension* Extension)
{
	if (IsValid(Extension))
	{
		// Should only add instances that have been created/renamed to us.
		check(Extension->GetOuter() == this);

		if (!Extensions.Contains(Extension->GetClass()))
		{
			Extensions.Add(Extension->GetClass(), Extension);
			Extension->PostExtensionAdded();
		}
	}

	return false;
}

void UHeartGraph::RemoveExtension(const TSubclassOf<UHeartGraphExtension> Class)
{
	if (const TObjectPtr<UHeartGraphExtension>* ExtensionPtr = Extensions.Find(Class))
	{
		if (*ExtensionPtr)
		{
			(*ExtensionPtr)->PreExtensionRemove();
		}
		Extensions.Remove(Class);
	}
}

UHeartGraphNode* UHeartGraph::CreateNodeForNodeObject(UObject* NodeObject, const FVector2D& Location)
{
	TSubclassOf<UHeartGraphNode> GraphNodeClass;

	if (auto&& RegistrySubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		GraphNodeClass = RegistrySubsystem->GetRegistry(GetClass())->GetGraphNodeClassForNode(FHeartNodeSource(NodeObject));
	}

	if (!IsValid(GraphNodeClass))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("GetGraphNodeClassForNode returned nullptr when trying to spawn node of class '%s'!"), *NodeObject->GetClass()->GetName())
		return nullptr;
	}

	auto&& NewGraphNode = NewObject<UHeartGraphNode>(this, GraphNodeClass);
	NewGraphNode->Guid = FHeartNodeGuid::New();
	NewGraphNode->Location = Location;
	NewGraphNode->NodeObject = NodeObject;

	NewGraphNode->OnCreate();

	return NewGraphNode;
}

UHeartGraphNode* UHeartGraph::CreateNodeForNodeClass(const UClass* NodeClass, const FVector2D& Location)
{
	TSubclassOf<UHeartGraphNode> GraphNodeClass;

	if (auto&& RegistrySubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		GraphNodeClass = RegistrySubsystem->GetRegistry(GetClass())->GetGraphNodeClassForNode(FHeartNodeSource(NodeClass));
	}

	if (!IsValid(GraphNodeClass))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("GetGraphNodeClassForNode returned nullptr when trying to spawn node of class '%s'!"), *NodeClass->GetName())
		return nullptr;
	}

	UHeartGraphNode* NewGraphNode = NewObject<UHeartGraphNode>(this, GraphNodeClass);
	NewGraphNode->Guid = FHeartNodeGuid::New();
	NewGraphNode->Location = Location;
	NewGraphNode->NodeObject = NewObject<UObject>(NewGraphNode, NodeClass);

	NewGraphNode->OnCreate();

	return NewGraphNode;
}

UHeartGraphNode* UHeartGraph::CreateNodeFromClass(const UClass* NodeClass, const FVector2D& Location)
{
	if (!ensure(IsValid(NodeClass)))
	{
		return nullptr;
	}

	return CreateNodeForNodeClass(NodeClass, Location);
}

UHeartGraphNode* UHeartGraph::CreateNodeFromObject(UObject* NodeObject, const FVector2D& Location)
{
	if (!ensure(IsValid(NodeObject)))
	{
		return nullptr;
	}

	return CreateNodeForNodeObject(NodeObject, Location);
}

void UHeartGraph::AddNode(UHeartGraphNode* Node)
{
	if (!ensure(IsValid(Node) && Node->GetGuid().IsValid()))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("Tried to add invalid node!"))
		return;
	}

	/*
	if (!ensure(IsValid(Node->GetNodeObject())))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("Tried to add node with invalid object!"))
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

	auto&& NodeBeingRemoved = Nodes.Find(NodeGuid);
	auto&& Removed = Nodes.Remove(NodeGuid);

	if (NodeBeingRemoved)
	{
		OnNodeRemoved.Broadcast(*NodeBeingRemoved);
	}

	return Removed > 0;
}

bool UHeartGraph::ConnectPins(const FHeartGraphPinReference& PinA, const FHeartGraphPinReference& PinB)
{
	UHeartGraphNode* ANode = GetNode(PinA.NodeGuid);
	UHeartGraphNode* BNode = GetNode(PinB.NodeGuid);

	if (!ensure(IsValid(ANode) && IsValid(BNode)))
	{
		return false;
	}

	// Add to both lists
	ANode->GetLinks(PinA.PinGuid).Links.Add(PinB);
	BNode->GetLinks(PinB.PinGuid).Links.Add(PinA);

	ANode->NotifyPinConnectionsChanged(PinA.PinGuid);
	BNode->NotifyPinConnectionsChanged(PinB.PinGuid);
	NotifyNodeConnectionsChanged({ANode, BNode}, {PinA.PinGuid, PinB.PinGuid});

	return true;
}

bool UHeartGraph::DisconnectPins(const FHeartGraphPinReference& PinA, const FHeartGraphPinReference& PinB)
{
	UHeartGraphNode* ANode = GetNode(PinA.NodeGuid);
	UHeartGraphNode* BNode = GetNode(PinB.NodeGuid);

	if (!ensureAlways(IsValid(ANode) && IsValid(BNode)))
	{
		return false;
	}

	auto& AConnections = ANode->GetLinks(PinA.PinGuid);
	auto& BConnections = BNode->GetLinks(PinB.PinGuid);

	// We assume that both of these are true, but proceed anyway if only one of them are...
	if (AConnections.Links.Contains(PinB) ||
		BConnections.Links.Contains(PinA))
	{
		AConnections.Links.Remove(PinB);
		BConnections.Links.Remove(PinA);

		ANode->NotifyPinConnectionsChanged(PinA.PinGuid);
		BNode->NotifyPinConnectionsChanged(PinB.PinGuid);
		NotifyNodeConnectionsChanged({ANode, BNode}, {PinA.PinGuid, PinB.PinGuid});
	}

	return true;
}

void UHeartGraph::DisconnectAllPins(const FHeartGraphPinReference& Pin)
{
	UHeartGraphNode* ANode = GetNode(Pin.NodeGuid);

	if (!ensure(IsValid(ANode)))
	{
		return;
	}

	const FHeartGraphPinConnections Connections = ANode->GetLinks(Pin.PinGuid);
	if (Connections.Links.IsEmpty()) return;

	for (const FHeartGraphPinReference& Link : Connections.Links)
	{
		DisconnectPins(Pin, Link);
	}
}

#undef LOCTEXT_NAMESPACE