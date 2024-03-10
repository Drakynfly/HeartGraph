// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphUtils.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphInterface.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPinInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphUtils)

namespace Heart::Utils
{
	UHeartGraphNode* FindNodeOfClass(const UHeartGraph* Graph, TSubclassOf<UHeartGraphNode> Class)
	{
		if (!IsValid(Class)) return nullptr;

		UHeartGraphNode* OutNode = nullptr;

		// Find first node of the given class
		Graph->ForEachNode(
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

	UHeartGraphNode* FindNodeByPredicate(const UHeartGraph* Graph, const FFindNodePredicate& Predicate)
	{
		if (UNLIKELY(!Predicate.IsBound())) return nullptr;

		UHeartGraphNode* OutNode = nullptr;

		// Find first node that returns true for the given predicate
		Graph->ForEachNode(
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

	TArray<UHeartGraphNode*> FindAllNodesOfClass(const UHeartGraph* Graph, TSubclassOf<UHeartGraphNode> Class)
	{
		check(Graph);
		if (!IsValid(Class)) return {};

		TArray<UHeartGraphNode*> OutNodes;

		// Find first node of the given class
		Graph->ForEachNode(
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

	TArray<UHeartGraphNode*> FindAllNodesByPredicate(const UHeartGraph* Graph, const FFindNodePredicate& Predicate)
	{
		TArray<UHeartGraphNode*> OutNodes;

		// Find first node of the given class
		Graph->ForEachNode(
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

	Query::FPinQueryResult FindPinsByTag(const UHeartGraphNode* Node, FHeartGraphPinTag Tag)
	{
		return Node->QueryPins().Filter(
			[Tag](const FHeartGraphPinDesc& Desc)
			{
				return Desc.Tag == Tag;
			});
	}

	TOptional<FHeartGraphPinDesc> ResolvePinDesc(const UHeartGraph* Graph, const FHeartGraphPinReference& Reference)
	{
		if (!IsValid(Graph))
		{
			return {};
		}

		auto&& Node = Graph->GetNode(Reference.NodeGuid);
		if (!IsValid(Node))
		{
			return {};
		}

		return Node->GetPinDesc(Reference.PinGuid);
	}
}

bool UHeartGraphUtils::Equal_HeartGuidHeartGuid(const FHeartGuid A, const FHeartGuid B)
{
	return A == B;
}

bool UHeartGraphUtils::NotEqual_HeartGuidHeartGuid(const FHeartGuid A, const FHeartGuid B)
{
	return A != B;
}

bool UHeartGraphUtils::Equal_PinReferencePinReference(const FHeartGraphPinReference& A, const FHeartGraphPinReference& B)
{
	return A == B;
}

bool UHeartGraphUtils::NotEqual_PinReferencePinReference(const FHeartGraphPinReference& A, const FHeartGraphPinReference& B)
{
	return A != B;
}

UHeartGraphNode* UHeartGraphUtils::FindNodeOfClass(const TScriptInterface<IHeartGraphInterface>& Graph,
												   const TSubclassOf<UHeartGraphNode> Class)
{
	if (!Graph.GetInterface()) return nullptr;
	return Heart::Utils::FindNodeOfClass(Graph->GetHeartGraph(), Class);
}

UHeartGraphNode* UHeartGraphUtils::FindNodeByPredicate(const TScriptInterface<IHeartGraphInterface>& Graph,
                                                       const FHeartGraphNodePredicate& Predicate)
{
	if (!Graph.GetInterface()) return nullptr;
	return Heart::Utils::FindNodeByPredicate(Graph->GetHeartGraph(), Heart::Utils::FFindNodePredicate::CreateLambda(
		[Predicate](const UHeartGraphNode* Node)
		{
			return Predicate.Execute(Node);
		}));
}

TArray<UHeartGraphNode*> UHeartGraphUtils::FindAllNodesOfClass(const TScriptInterface<IHeartGraphInterface>& Graph,
															   const TSubclassOf<UHeartGraphNode> Class)
{
	if (!Graph.GetInterface()) return {};
	return Heart::Utils::FindAllNodesOfClass(Graph->GetHeartGraph(), Class);
}

TArray<UHeartGraphNode*> UHeartGraphUtils::FindAllNodesByPredicate(const TScriptInterface<IHeartGraphInterface>& Graph,
                                                                   const FHeartGraphNodePredicate& Predicate)
{
	if (!Graph.GetInterface()) return {};
	return Heart::Utils::FindAllNodesByPredicate(Graph->GetHeartGraph(), Heart::Utils::FFindNodePredicate::CreateLambda(
		[Predicate](const UHeartGraphNode* Node)
		{
			return Predicate.Execute(Node);
		}));
}

TArray<FHeartPinGuid> UHeartGraphUtils::FindPinsByTag(const UHeartGraphNode* Node, const FHeartGraphPinTag Tag)
{
	return Heart::Utils::FindPinsByTag(Node, Tag).Get();
}

bool UHeartGraphUtils::WouldConnectionCreateLoop(const UHeartGraphNode* A, const UHeartGraphNode* B)
{
	// This is based on the engine class FNodeVisitorCycleChecker found in both EdGraphSchema_BehaviorTree.cpp and ConversationGraphSchema.cpp

	class FNodeVisitorCycleChecker
	{
	public:
		/** Check whether a loop in the graph would be caused by linking the passed-in nodes */
		bool CheckForLoop(const UHeartGraphNode* StartNode, const UHeartGraphNode* EndNode)
		{
			VisitedNodes.Add(EndNode);
			return TraverseInputNodesToRoot(StartNode);
		}

	private:
		/**
		 * Helper function for CheckForLoop()
		 * @param	Node	The node to start traversal at
		 * @return true if we reached a root node (i.e. a node with no input pins), false if we encounter a node we have already seen
		 */
		bool TraverseInputNodesToRoot(const UHeartGraphNode* Node)
		{
			VisitedNodes.Add(Node);

			const UHeartGraph* Graph = Node->GetGraph();

			for (auto&& Inputs = Node->GetInputPins();
				 auto Input : Inputs)
			{
				auto&& Connections = Node->GetConnections(Input);
				if (!Connections.IsSet())
				{
					continue;
				}

				for (auto&& Link : Connections.GetValue().Links)
				{
					if (Link.IsValid())
					{
						const UHeartGraphNode* OtherNode = Graph->GetNode(Link.NodeGuid);
						if (VisitedNodes.Contains(OtherNode))
						{
							return false;
						}
						return TraverseInputNodesToRoot(OtherNode);
					}
				}
			}

			return true;
		}

		TSet<const UHeartGraphNode*> VisitedNodes;
	};

	return !FNodeVisitorCycleChecker().CheckForLoop(A, B);
}

bool UHeartGraphUtils::GetGraphTyped(const TScriptInterface<IHeartGraphNodeInterface>& Node, TSubclassOf<UHeartGraph> Class, UHeartGraph*& Graph)
{
	if (Node.GetInterface())
	{
		Graph = Node->GetHeartGraphNode()->GetGraph();
	}
	return IsValid(Graph);
}

bool UHeartGraphUtils::GetGraphNodeTyped(const TScriptInterface<IHeartGraphPinInterface>& Pin, TSubclassOf<UHeartGraphNode> Class, UHeartGraphNode*& Node)
{
	if (Pin.GetInterface())
	{
		Node = Pin->GetHeartGraphNode();
	}
	return IsValid(Node);
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

TOptional<FHeartGraphPinDesc> UHeartGraphUtils::ResolvePinDesc(const TScriptInterface<IHeartGraphInterface>& Graph, const FHeartGraphPinReference& Reference)
{
	return Heart::Utils::ResolvePinDesc(Graph->GetHeartGraph(), Reference);
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