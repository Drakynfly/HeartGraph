// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphUtils.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphInterface.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPinInterface.h"
#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphUtils)

namespace Heart::Utils
{
	UHeartGraphNode* FindNodeOfClass(const UHeartGraph* Graph, TSubclassOf<UHeartGraphNode> Class)
	{
		if (!IsValid(Class)) return nullptr;

		UHeartGraphNode* OutNode = nullptr;

		// Find the first node of the given class
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
		UHeartGraphNode* OutNode = nullptr;

		// Find the first node that returns true for the given predicate
		Graph->ForEachNode(
			[Predicate, &OutNode](UHeartGraphNode* Node)
			{
				if (Predicate(Node))
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

		// Find the first node of the given class
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

		// Find the first node of the given class
		Graph->ForEachNode(
			[Predicate, &OutNodes](UHeartGraphNode* Node)
			{
				if (Predicate(Node))
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

	TArray<FHeartNodeGuid> GetConnectedNodes(const UHeartGraph* Graph, const FHeartNodeGuid& Node, const EHeartPinDirection Direction)
	{
		if (!ensure(IsValid(Graph) && Node.IsValid())) return {};

		TSet<FHeartNodeGuid> UniqueConnections;
		const UHeartGraphNode* GraphNode = Graph->GetNode(Node);
		if (!IsValid(GraphNode))
		{
			return {};
		}

		GraphNode->FindPinsByDirection(Direction)
			.ForEach(
			[&](const FHeartPinGuid PinGuid)
			{
				auto&& LinksView = GraphNode->ViewConnections(PinGuid);
				if (!LinksView.IsValid())
				{
					return;
				}

				const TConstArrayView<FHeartGraphPinReference> LinksArrayView = LinksView.Get().GetLinks();
				UniqueConnections.Reserve(LinksArrayView.Num());

				for (auto&& Link : LinksArrayView)
				{
					UniqueConnections.Add(Link.NodeGuid);
				}
			});

		return UniqueConnections.Array();
	}

	TConstStructView<FHeartGraphPinDesc> ResolvePinReference(const UHeartGraph* Graph, const FHeartGraphPinReference& Reference)
	{
		if (!IsValid(Graph))
		{
			return TConstStructView<FHeartGraphPinDesc>();
		}

		auto&& Node = Graph->GetNode(Reference.NodeGuid);
		if (!IsValid(Node))
		{
			return TConstStructView<FHeartGraphPinDesc>();
		}

		return Node->ViewPin(Reference.PinGuid);
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
	return Heart::Utils::FindNodeByPredicate(Graph->GetHeartGraph(),
		[Predicate](const UHeartGraphNode* Node)
		{
			return Predicate.Execute(Node);
		});
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
	return Heart::Utils::FindAllNodesByPredicate(Graph->GetHeartGraph(),
		[Predicate](const UHeartGraphNode* Node)
		{
			return Predicate.Execute(Node);
		});
}

TArray<FHeartPinGuid> UHeartGraphUtils::FindPinsByTag(const UHeartGraphNode* Node, const FHeartGraphPinTag Tag)
{
	return Heart::Utils::FindPinsByTag(Node, Tag).Get();
}

TArray<FHeartNodeGuid> UHeartGraphUtils::GetConnectedNodes(const UHeartGraph* Graph, const FHeartNodeGuid& Node,
	const EHeartPinDirection Direction)
{
	return Heart::Utils::GetConnectedNodes(Graph, Node, Direction);
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
			VisitedNodes.Add(EndNode->GetGuid());
			return TraverseInputNodesToRoot(*StartNode->GetGraph(), StartNode->GetGuid());
		}

	private:
		/**
		 * Helper function for CheckForLoop()
		 * @param	Node	The node to start traversal at
		 * @return true if we reached a root node (i.e. a node with no input pins), false if we encounter a node we have already seen
		 */
		bool TraverseInputNodesToRoot(const UHeartGraph& Graph, const FHeartNodeGuid& Node)
		{
			VisitedNodes.Add(Node);

			TArray<FHeartNodeGuid> LinkedNodes = Heart::Utils::GetConnectedNodes(
				&Graph, Node, EHeartPinDirection::Input);

			for (auto&& LinkedNode : LinkedNodes)
			{
				if (VisitedNodes.Contains(LinkedNode))
				{
					return false;
				}
				return TraverseInputNodesToRoot(Graph, LinkedNode);
			}

			return true;
		}

		TSet<FHeartNodeGuid> VisitedNodes;
	};

	return !FNodeVisitorCycleChecker().CheckForLoop(A, B);
}

UHeartGraphExtension* UHeartGraphUtils::FindExtension(const TScriptInterface<IHeartGraphInterface>& Graph, const TSubclassOf<UHeartGraphExtension> Class)
{
	if (Graph.GetInterface())
	{
		return Graph->GetHeartGraph()->GetExtension(Class);
	}
	return nullptr;
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
	auto PinView = Heart::Utils::ResolvePinReference(Graph->GetHeartGraph(), Reference);
	if (PinView.IsValid())
	{
		return PinView.Get();
	}
	return NullOpt;
}

void UHeartGraphUtils::BreakHeartActionRecord(const FHeartActionRecord& Record, TSubclassOf<UHeartActionBase>& Action,
											  UObject*& Target, FHeartInputActivation& Activation, UObject*& Payload,
											  FBloodContainer& UndoData)
{
	Action = Record.Action;
	Target = Record.Arguments.Target;
	Activation = Record.Arguments.Activation;
	Payload = Record.Arguments.Payload;
	UndoData = Record.UndoData;
}

FHeartNodeSource UHeartGraphUtils::MakeNodeSourceFromClass(UClass* Class)
{
	return FHeartNodeSource(Class);
}

FHeartNodeSource UHeartGraphUtils::MakeNodeSourceFromObject(UObject* Object)
{
	return FHeartNodeSource(Object);
}

const UClass* UHeartGraphUtils::NodeSourceToClass(const FHeartNodeSource& NodeSource, const UClass* BaseClass)
{
	if (BaseClass == nullptr ||
		BaseClass == UObject::StaticClass() ||
		NodeSource.IsAOrClassOf(BaseClass))
	{
		return NodeSource.As<UClass>();
	}
	return nullptr;
}

const UObject* UHeartGraphUtils::NodeSourceToObject(const FHeartNodeSource& NodeSource, const UClass* BaseClass)
{
	if (BaseClass == nullptr ||
		BaseClass == UObject::StaticClass() ||
		NodeSource.IsAOrClassOf(BaseClass))
	{
		return NodeSource.As<UObject>();
	}
	return nullptr;
}