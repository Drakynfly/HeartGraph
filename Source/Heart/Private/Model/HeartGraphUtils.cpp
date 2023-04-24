﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphUtils.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPinInterface.h"

UHeartGraphNode* UHeartGraphUtils::FindNodeOfClass(const TScriptInterface<IHeartGraphInterface> Graph,
                                                   TSubclassOf<UHeartGraphNode> Class)
{
	if (!Graph.GetInterface()) return nullptr;
	if (!IsValid(Class)) return nullptr;

	UHeartGraphNode* OutNode = nullptr;

	// Find first node of the given class
	Graph->GetHeartGraph()->ForEachNode(
		[Class, &OutNode](UHeartGraphNode* Node)
		{
			if (Node->IsA(Class))
			{
				OutNode = Node;
			}
			return IsValid(OutNode);
		});

	return OutNode;
}

UHeartGraphNode* UHeartGraphUtils::FindNodeByPredicate(const TScriptInterface<IHeartGraphInterface> Graph,
                                                       const FHeartGraphNodePredicate& Predicate)
{
	if (!Graph.GetInterface()) return nullptr;
	if (!Predicate.IsBound()) return nullptr;

	UHeartGraphNode* OutNode = nullptr;

	// Find first node that returns true for the given predicate
	Graph->GetHeartGraph()->ForEachNode(
		[Predicate, &OutNode](UHeartGraphNode* Node)
		{
			if (Predicate.Execute(Node))
			{
				OutNode = Node;
			}
			return IsValid(OutNode);
		});

	return OutNode;
}

TArray<UHeartGraphNode*> UHeartGraphUtils::FindAllNodesOfClass(const TScriptInterface<IHeartGraphInterface> Graph,
                                                               TSubclassOf<UHeartGraphNode> Class)
{
	if (!Graph.GetInterface()) return {};
	if (!IsValid(Class)) return {};

	TArray<UHeartGraphNode*> OutNodes;

	// Find first node of the given class
	Graph->GetHeartGraph()->ForEachNode(
		[Class, &OutNodes](UHeartGraphNode* Node)
		{
			if (Node->IsA(Class))
			{
				OutNodes.Add(Node);
			}
			return false;
		});

	return OutNodes;
}

TArray<UHeartGraphNode*> UHeartGraphUtils::FindAllNodesByPredicate(const TScriptInterface<IHeartGraphInterface> Graph,
                                                                   const FHeartGraphNodePredicate& Predicate)
{
	if (!Graph.GetInterface()) return {};

	TArray<UHeartGraphNode*> OutNodes;

	// Find first node of the given class
	Graph->GetHeartGraph()->ForEachNode(
		[Predicate, &OutNodes](UHeartGraphNode* Node)
		{
			if (Predicate.Execute(Node))
			{
				OutNodes.Add(Node);
			}
			return false;
		});

	return OutNodes;
}

bool UHeartGraphUtils::GetGraphTyped(const TScriptInterface<IHeartGraphNodeInterface> Node, TSubclassOf<UHeartGraph> Class, UHeartGraph*& Graph)
{
	if (Node.GetInterface())
	{
		Graph = Node->GetHeartGraphNode()->GetGraph();
	}
	return IsValid(Graph);
}

bool UHeartGraphUtils::GetNodeObjectTyped(const TScriptInterface<IHeartGraphNodeInterface> Node, TSubclassOf<UObject> Class,
                                          UObject*& Object)
{
	if (Node.GetInterface())
	{
		Object = Node->GetHeartGraphNode()->GetNodeObject();
	}
	return IsValid(Object);
}

bool UHeartGraphUtils::GetNodeTyped(const TScriptInterface<IHeartGraphPinInterface> Pin, TSubclassOf<UHeartGraphNode> Class, UHeartGraphNode*& Node)
{
	if (Pin.GetInterface())
	{
		Node = Pin->GetNode();
	}
	return IsValid(Node);
}