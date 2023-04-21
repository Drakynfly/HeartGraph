// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartCanvasUtils.h"
#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"

bool UHeartCanvasUtils::GetGraphTyped(UHeartGraphCanvas* Canvas, TSubclassOf<UHeartGraph> Class, UHeartGraph*& Graph)
{
	Graph = Canvas->GetGraph();
	return IsValid(Graph);
}

bool UHeartCanvasUtils::GetNodeTyped(UHeartGraphCanvasNode* CanvasNode, TSubclassOf<UHeartGraphNode> Class,
									 UHeartGraphNode*& Node)
{
	Node = CanvasNode->GetNode();
	return IsValid(Node);
}
