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

UHeartGraphNode* UHeartGraph::CreateNode(const TSubclassOf<UObject> NodeClass, const FVector2D& Location)
{
	if (!ensure(IsValid(NodeClass)))
	{
		return nullptr;
	}

	auto&& Schema = GetSchema();

	if (!ensure(IsValid(Schema)))
	{
		return nullptr;
	}

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

	auto&& NewGraphNode = NewObject<UHeartGraphNode>(this, GraphNodeClass);
	NewGraphNode->Guid = FHeartNodeGuid::NewGuid();
	NewGraphNode->Location = Location;
	NewGraphNode->NodeObject = NewObject<UObject>(NewGraphNode, NodeClass);

	return NewGraphNode;
}

void UHeartGraph::AddNode(UHeartGraphNode* Node)
{
	if (!ensure(IsValid(Node) && Node->GetGuid().IsValid()))
	{
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
		HeartEdGraph->AddNode(Node->GetEdGraphNode());
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