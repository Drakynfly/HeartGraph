// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraph.h"
#include "ModelView//HeartGraphBehavior.h"
#include "ModelView/HeartGraphNode.h"

TSubclassOf<UHeartGraphBehavior> UHeartGraph::GetBehaviorClass_Implementation() const
{
	return UHeartGraphBehavior::StaticClass();
}

const UHeartGraphBehavior* UHeartGraph::GetBehavior() const
{
	return GetDefault<UHeartGraphBehavior>(GetBehaviorClass());
}

const UHeartGraphBehavior* UHeartGraph::GetBehaviorTyped_K2(TSubclassOf<UHeartGraphBehavior>) const
{
	return GetBehavior();
}

UHeartGraphNode* UHeartGraph::GetNode(const FHeartNodeGuid& NodeGuid)
{
	auto Result = Nodes.Find(NodeGuid);
	return Result ? *Result : nullptr;
}

const UHeartGraphBehavior* UHeartGraph::GetBehaviorStatic(const TSubclassOf<UHeartGraph> HeartGraphClass)
{
	return GetDefault<UHeartGraph>(HeartGraphClass)->GetBehavior();
}

void UHeartGraph::AddNode(UHeartGraphNode* Node)
{
	if (!ensure(IsValid(Node) && Node->GetGuid().IsValid()))
	{
		return;
	}

	Nodes.Add(Node->GetGuid(), Node);
}

bool UHeartGraph::RemoveNode(UHeartGraphNode* Node)
{
	if (!ensure(IsValid(Node) && Node->GetGuid().IsValid()))
	{
		return false;
	}

	const auto Removed = Nodes.Remove(Node->GetGuid());

	return Removed > 0;
}