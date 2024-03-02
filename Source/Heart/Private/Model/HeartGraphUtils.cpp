// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphUtils.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphInterface.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPinInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphUtils)

bool UHeartGraphUtils::Equal_HeartGuidHeartGuid(const FHeartGuid A, const FHeartGuid B)
{
	return A == B;
}

bool UHeartGraphUtils::NotEqual_HeartGuidHeartGuid(const FHeartGuid A, const FHeartGuid B)
{
	return A != B;
}

bool UHeartGraphUtils::Equal_PinReferencePinReference(FHeartGraphPinReference A, FHeartGraphPinReference B)
{
	return A == B;
}

bool UHeartGraphUtils::NotEqual_PinReferencePinReference(FHeartGraphPinReference A, FHeartGraphPinReference B)
{
	return A != B;
}

UHeartGraphNode* UHeartGraphUtils::FindNodeOfClass(const TScriptInterface<IHeartGraphInterface>& Graph,
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

UHeartGraphNode* UHeartGraphUtils::FindNodeByPredicate(const TScriptInterface<IHeartGraphInterface>& Graph,
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

TArray<UHeartGraphNode*> UHeartGraphUtils::FindAllNodesOfClass(const TScriptInterface<IHeartGraphInterface>& Graph,
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
			return true;
		});

	return OutNodes;
}

TArray<UHeartGraphNode*> UHeartGraphUtils::FindAllNodesByPredicate(const TScriptInterface<IHeartGraphInterface>& Graph,
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
			return true;
		});

	return OutNodes;
}

bool UHeartGraphUtils::GetGraphTyped(const TScriptInterface<IHeartGraphNodeInterface>& Node, TSubclassOf<UHeartGraph> Class, UHeartGraph*& Graph)
{
	if (Node.GetInterface())
	{
		Graph = Node->GetHeartGraphNode()->GetGraph();
	}
	return IsValid(Graph);
}

bool UHeartGraphUtils::GetNodeObjectTyped(const TScriptInterface<IHeartGraphNodeInterface>& Node, TSubclassOf<UObject> Class,
                                          UObject*& Object)
{
	if (Node.GetInterface())
	{
		Object = Node->GetHeartGraphNode()->GetNodeObject();
	}
	return IsValid(Object);
}

FHeartGraphPinReference UHeartGraphUtils::MakeReference(const TScriptInterface<IHeartGraphNodeInterface>& Node,
                                                        const TScriptInterface<IHeartGraphPinInterface>& Pin)
{
	return FHeartGraphPinReference{Node->GetHeartGraphNode()->GetGuid(), Pin->GetPinGuid()};
}

bool UHeartGraphUtils::GetGraphNodeTyped(const TScriptInterface<IHeartGraphPinInterface>& Pin, TSubclassOf<UHeartGraphNode> Class, UHeartGraphNode*& Node)
{
	if (Pin.GetInterface())
	{
		Node = Pin->GetHeartGraphNode();
	}
	return IsValid(Node);
}

FHeartNodeSource UHeartGraphUtils::MakeNodeSourceFromClass(UClass* Class)
{
	return FHeartNodeSource(Class);
}

FHeartNodeSource UHeartGraphUtils::MakeNodeSourceFromObject(UObject* Object)
{
	return FHeartNodeSource(Object);
}

UClass* UHeartGraphUtils::NodeSourceToClass(const FHeartNodeSource NodeSource, const UClass* BaseClass)
{
	if (BaseClass == nullptr ||
		BaseClass == UObject::StaticClass() ||
		NodeSource.IsAOrClassOf(BaseClass))
	{
		return NodeSource.As<UClass>();
	}
	return nullptr;
}

UObject* UHeartGraphUtils::NodeSourceToObject(const FHeartNodeSource NodeSource, const UClass* BaseClass)
{
	if (BaseClass == nullptr ||
		BaseClass == UObject::StaticClass() ||
		NodeSource.IsAOrClassOf(BaseClass))
	{
		return NodeSource.As<UObject>();
	}
	return nullptr;
}