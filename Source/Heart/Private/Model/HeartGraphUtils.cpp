// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphUtils.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPinInterface.h"

bool UHeartGraphUtils::GetGraphTyped(const TScriptInterface<IHeartGraphNodeInterface> Node, TSubclassOf<UHeartGraph> Class, UHeartGraph*& Graph)
{
	if (Node.GetInterface())
	{
		Graph = Node->GetHeartGraphNode()->GetGraph();
	}
	return IsValid(Graph);
}

bool UHeartGraphUtils::GetNodeTyped(const TScriptInterface<IHeartGraphPinInterface> Pin, TSubclassOf<UHeartGraphNode> Class, UHeartGraphNode*& Node)
{
	if (Pin.GetInterface())
	{
		Node = Pin->GetNode();
	}
	return IsValid(Node);
}
