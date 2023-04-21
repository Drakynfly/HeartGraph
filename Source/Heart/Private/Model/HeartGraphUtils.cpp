// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphUtils.h"
#include "Model/HeartGraphNode.h"

bool UHeartGraphUtils::GetGraphTyped(UHeartGraphNode* Node, TSubclassOf<UHeartGraph> Class, UHeartGraph*& Graph)
{
	Graph = Node->GetGraph();
	return IsValid(Graph);
}

bool UHeartGraphUtils::GetNodeTyped(UHeartGraphPin* Pin, TSubclassOf<UHeartGraphNode> Class, UHeartGraphNode*& Node)
{
	Node = Pin->GetNode();
	return IsValid(Node);
}
