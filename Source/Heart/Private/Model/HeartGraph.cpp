// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraph.h"
#include "ModelView/HeartGraphSchema.h"
#include "ModelView/HeartGraphNode.h"

DEFINE_LOG_CATEGORY(LogHeartGraph)

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

UHeartGraphNode* UHeartGraph::GetNode(const FHeartNodeGuid& NodeGuid) const
{
	auto&& Result = Nodes.Find(NodeGuid);
	return Result ? *Result : nullptr;
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
	OnNodeAdded.Broadcast(Node);
}

bool UHeartGraph::RemoveNode(UHeartGraphNode* Node)
{
	if (!ensure(IsValid(Node) && Node->GetGuid().IsValid()))
	{
		return false;
	}

	const auto Removed = Nodes.Remove(Node->GetGuid());

	if (Removed > 0)
	{
		OnNodeRemoved.Broadcast(Node);
	}

	return Removed > 0;
}