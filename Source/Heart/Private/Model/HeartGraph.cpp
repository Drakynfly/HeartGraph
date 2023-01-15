// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraph.h"

#include "Model/HeartGraphNode.h"
#include "ModelView/HeartGraphSchema.h"

#include "GraphRegistry/HeartNodeRegistrySubsystem.h"

DEFINE_LOG_CATEGORY(LogHeartGraph)

void UHeartGraph::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);

#if WITH_EDITOR
	// The HeartEdGraph doesn't need to persist for graphs duplicated during gameplay
	if (GetWorld()->IsGameWorld())
	{
		HeartEdGraph = nullptr;
	}
#endif
}

void UHeartGraph::NotifyNodeConnectionsChanged(const TArray<UHeartGraphNode*>& AffectedNodes, const TArray<UHeartGraphPin*>& AffectedPins)
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

#if WITH_EDITOR
void UHeartGraph::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	// Save the EdGraph with us in the editor
	UHeartGraph* This = CastChecked<UHeartGraph>(InThis);
	Collector.AddReferencedObject(This->HeartEdGraph, This);

	Super::AddReferencedObjects(InThis, Collector);
}
#endif

UHeartGraphNode* UHeartGraph::GetNode(const FHeartNodeGuid& NodeGuid) const
{
	auto&& Result = Nodes.Find(NodeGuid);
	return Result ? *Result : nullptr;
}

UHeartGraphNode* UHeartGraph::FindNodeOfClass(const TSubclassOf<UHeartGraphNode> Class)
{
	if (!IsValid(Class))
	{
		return nullptr;
	}

	for (auto&& Node : Nodes)
	{
		if (IsValid(Node.Value) && Node.Value.IsA(Class))
		{
			return Node.Value;
		}
	}

	return nullptr;
}

UHeartGraphNode* UHeartGraph::FindNodeByPredicate(const FHeartGraphNodePredicate& Predicate)
{
	for (auto&& Node : Nodes)
	{
		if (IsValid(Node.Value) && Predicate.Execute(Node.Value))
		{
			return Node.Value;
		}
	}

	return nullptr;
}

TArray<UHeartGraphNode*> UHeartGraph::FindAllNodesOfClass(const TSubclassOf<UHeartGraphNode> Class)
{
	if (!IsValid(Class))
	{
		return TArray<UHeartGraphNode*>();
	}

	TArray<UHeartGraphNode*> OutNodes;

	for (auto&& Node : Nodes)
	{
		if (IsValid(Node.Value) && Node.Value.IsA(Class))
		{
			OutNodes.Add(Node.Value);
		}
	}

	return OutNodes;
}

TArray<UHeartGraphNode*> UHeartGraph::FindAllNodesByPredicate(const FHeartGraphNodePredicate& Predicate)
{
	TArray<UHeartGraphNode*> OutNodes;

	for (auto&& Node : Nodes)
	{
		if (IsValid(Node.Value) && Predicate.Execute(Node.Value))
		{
			OutNodes.Add(Node.Value);
		}
	}

	return OutNodes;
}

TSubclassOf<UHeartGraphSchema> UHeartGraph::GetSchemaClass_Implementation() const
{
	return UHeartGraphSchema::StaticClass();
}

const UHeartGraphSchema* UHeartGraph::GetSchemaStatic(const TSubclassOf<UHeartGraph> HeartGraphClass)
{
	return GetDefault<UHeartGraph>(HeartGraphClass)->GetSchema();
}

const UHeartGraphSchema* UHeartGraph::GetSchema() const
{
	return GetDefault<UHeartGraphSchema>(GetSchemaClass());
}

const UHeartGraphSchema* UHeartGraph::GetSchemaTyped_K2(TSubclassOf<UHeartGraphSchema> Class) const
{
	return GetSchema();
}

UHeartGraphNode* UHeartGraph::CreateNodeForNodeObject(UObject* NodeObject, const FVector2D& Location)
{
	TSubclassOf<UHeartGraphNode> GraphNodeClass;

	if (auto&& NodeRegistry = GEngine->GetEngineSubsystem<UHeartNodeRegistrySubsystem>())
	{
		GraphNodeClass = NodeRegistry->GetRegistry(GetClass())->GetGraphNodeClassForNode(NodeObject->GetClass());
	}

	if (!IsValid(GraphNodeClass))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("GetGraphNodeClassForNode returned nullptr when trying to spawn node of class: %s!"), *NodeObject->GetClass()->GetName())
		return nullptr;
	}

	auto&& NewGraphNode = NewObject<UHeartGraphNode>(this, GraphNodeClass);
	NewGraphNode->Guid = FHeartNodeGuid::NewGuid();
	NewGraphNode->Location = Location;
	NewGraphNode->NodeObject = NodeObject;

	NewGraphNode->OnCreate();

	return NewGraphNode;
}

UHeartGraphNode* UHeartGraph::CreateNodeForNodeClass(const UClass* NodeClass, const FVector2D& Location)
{
	TSubclassOf<UHeartGraphNode> GraphNodeClass;

	if (auto&& NodeRegistry = GEngine->GetEngineSubsystem<UHeartNodeRegistrySubsystem>())
	{
		GraphNodeClass = NodeRegistry->GetRegistry(GetClass())->GetGraphNodeClassForNode(NodeClass);
	}

	if (!IsValid(GraphNodeClass))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("GetGraphNodeClassForNode returned nullptr when trying to spawn node of class: %s!"), *NodeClass->GetName())
		return nullptr;
	}

	// The graph has to be the outer for the NodeObjects or Unreal will kill them when recompiling the heart graph blueprint
	// This is probably just a issue with the current version of unreal (5.1.0), but even if it's fixed, it's probably fine
	// to leave it like this.
	auto&& NewNodeObject = NewObject<UObject>(this, NodeClass);

	auto&& NewGraphNode = NewObject<UHeartGraphNode>(this, GraphNodeClass);
	NewGraphNode->Guid = FHeartNodeGuid::NewGuid();
	NewGraphNode->Location = Location;
	NewGraphNode->NodeObject = NewNodeObject;

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

	if (!ensure(IsValid(Node->GetNodeObject())))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("Tried to add node with invalid object!"))
		return;
	}

	const FHeartNodeGuid NodeGuid = Node->GetGuid();

	if (Nodes.Contains(NodeGuid))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("Tried to add node already in graph!"))
		return;
	}

	Nodes.Add(NodeGuid, Node);

#if WITH_EDITOR
	if (HeartEdGraph)
	{
		if (Node->GetEdGraphNode())
		{
			HeartEdGraph->AddNode(Node->GetEdGraphNode());
		}
		else
		{
			OnNodeCreatedInEditorExternally.ExecuteIfBound(Node);
		}
	}
#endif

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