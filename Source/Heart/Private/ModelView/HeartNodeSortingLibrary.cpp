// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartNodeSortingLibrary.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphUtils.h"
#include "Model/HeartQueries.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNodeSortingLibrary)

TArray<UHeartGraphNode*> UHeartNodeSortingLibrary::ResolveNodes(const UHeartGraph* Graph, const TArray<FHeartNodeGuid>& Nodes)
{
	TArray<UHeartGraphNode*> Out;
	Out.Reserve(Nodes.Num());
	for (auto&& Guid : Nodes)
	{
		if (UHeartGraphNode* Node = Graph->GetNode(Guid))
		{
			Out.Add(Node);
		}
	}
	return Out;
}

TArray<FHeartNodeGuid> UHeartNodeSortingLibrary::SortNodes(const TArray<FHeartNodeGuid>& Nodes,
															 const FHeartNodeComparePredicate& Predicate)
{
	if (!ensure(Predicate.IsBound()))
	{
		return TArray<FHeartNodeGuid>();
	}

	TArray<FHeartNodeGuid> OutNodes = Nodes;
	SortNodesInPlace(OutNodes, Predicate);
	return OutNodes;
}

void UHeartNodeSortingLibrary::SortNodesInPlace(TArray<FHeartNodeGuid>& Nodes, const FHeartNodeComparePredicate& Predicate)
{
	if (!ensure(Predicate.IsBound()))
	{
		return;
	}

	Algo::Sort(Nodes, [&Predicate](const FHeartNodeGuid& A, const FHeartNodeGuid& B)
		{
			return Predicate.Execute(A, B);
		});
}

TArray<FHeartNodeGuid> UHeartNodeSortingLibrary::FilterNodesByPredicate(const TArray<FHeartNodeGuid>& Nodes,
                                                                          const FHeartNodeFilterPredicate& Predicate)
{
	if (!ensure(Predicate.IsBound()))
	{
		return TArray<FHeartNodeGuid>();
	}

	return Nodes.FilterByPredicate([&Predicate](const FHeartNodeGuid& Node)
		{
			return Predicate.Execute(Node);
		});
}

TArray<FHeartNodeGuid> UHeartNodeSortingLibrary::FilterNodesByPredicate_Exclusive(
	const TArray<FHeartNodeGuid>& Nodes, const FHeartNodeFilterPredicate& Predicate)
{
	if (!ensure(Predicate.IsBound()))
	{
		return TArray<FHeartNodeGuid>();
	}

	return Nodes.FilterByPredicate([&Predicate](const FHeartNodeGuid& Node)
		{
			return !Predicate.Execute(Node);
		});
}

TArray<FHeartNodeGuid> UHeartNodeSortingLibrary::FilterNodesByClass(const TScriptInterface<IHeartGraphInterface> Graph,
																	const TArray<FHeartNodeGuid>& Nodes,
                                                                    const TSet<TSubclassOf<UHeartGraphNode>>& Classes)
{
	if (!ensure(!Classes.IsEmpty()))
	{
		return TArray<FHeartNodeGuid>();
	}

	const UHeartGraph* HeartGraph = Graph->GetHeartGraph();
	if (IsValid(HeartGraph))
	{
		return TArray<FHeartNodeGuid>();
	}

	return Nodes.FilterByPredicate([&Classes, HeartGraph](const FHeartNodeGuid& Node)
	{
		const UHeartGraphNode* NodePtr = HeartGraph->GetNode(Node);
		if (!IsValid(NodePtr))
		{
			return false;
		}

		for (auto&& Class = NodePtr->GetClass(); Class && Class != UObject::StaticClass(); Class = Class->GetSuperClass())
		{
			if (Classes.Contains(Class))
			{
				return true;
			}
		}
		return false;
	});
}

TArray<FHeartNodeGuid> UHeartNodeSortingLibrary::FilterNodesByClass_Exclusive(const TScriptInterface<IHeartGraphInterface> Graph,
																			  const TArray<FHeartNodeGuid>& Nodes,
                                                                              const TSet<TSubclassOf<UHeartGraphNode>>& Classes)
{
	if (!ensure(!Classes.IsEmpty()))
	{
		return TArray<FHeartNodeGuid>();
	}

	const UHeartGraph* HeartGraph = Graph->GetHeartGraph();
	if (IsValid(HeartGraph))
	{
		return TArray<FHeartNodeGuid>();
	}

	return Nodes.FilterByPredicate([&Classes, HeartGraph](const FHeartNodeGuid& Node)
		{
			const UHeartGraphNode* NodePtr = HeartGraph->GetNode(Node);
			if (!IsValid(NodePtr))
			{
				return false;
			}

			for (auto&& Class = NodePtr->GetClass(); Class && Class != UObject::StaticClass(); Class = Class->GetSuperClass())
			{
				if (Classes.Contains(Class))
				{
					return false;
				}
			}
			return true;
		});
}

void UHeartNodeSortingLibrary::SortLooseNodesIntoTrees(const TScriptInterface<IHeartGraphInterface> Graph, const TArray<FHeartNodeGuid>& Nodes, const FNodeLooseToTreeArgs& Args, TArray<FHeartTree>& Trees)
{
	const UHeartGraph* HeartGraph = Graph->GetHeartGraph();
	if (IsValid(HeartGraph))
	{
		return;
	}

	EHeartPinDirection InverseDirection = Args.Direction == EHeartPinDirection::Input ? EHeartPinDirection::Output : EHeartPinDirection::Input;

	TSet<const UHeartGraphNode*> TrackedNodes;

	// Recursive function for building tree nodes
	TFunction<FHeartTreeNode(const UHeartGraphNode*)> BuildTreeNode;
	BuildTreeNode = [InverseDirection, &BuildTreeNode, &TrackedNodes, AllowDuplicates = Args.AllowDuplicates](const UHeartGraphNode* Node)
		{
			FHeartTreeNode OutTreeNode;
			OutTreeNode.Node = Node->GetGuid();

			const TArray<FHeartNodeGuid> OutputLinks = Heart::Utils::GetConnectedNodes(Node->GetGraph(), OutTreeNode.Node, InverseDirection);

			for (auto&& OutputLink : OutputLinks)
			{
				if (auto&& ConnectedNode = Node->GetGraph()->GetNode(OutputLink))
				{
					if (TrackedNodes.Contains(ConnectedNode))
					{
						if (!AllowDuplicates)
						{
							continue;
						}
					}
					else
					{
						TrackedNodes.Add(ConnectedNode);
					}

					OutTreeNode.Children.Add(TInstancedStruct<FHeartTreeNode>::Make(BuildTreeNode(ConnectedNode)));
				}
			}

			return OutTreeNode;
		};


	for (const FHeartNodeGuid& NodeGuid : Nodes)
	{
		const UHeartGraphNode* Node = HeartGraph->GetNode(NodeGuid);
		if (!IsValid(Node))
		{
			continue;
		}

		// Filter by nodes that have no connections in the specified direction
		if (Node->FindPinsByPredicate(Args.Direction,
				[Node](const FHeartPinGuid Pin, const FHeartGraphPinDesc&)
				{
					return Node->HasConnections(Pin);
				}).Num() == 0)
		{
			Trees.Add(FHeartTree(HeartGraph, BuildTreeNode(Node)));
		}
	}
}

void UHeartNodeSortingLibrary::ConvertNodeTreeToLayers(const FHeartTree& Tree, TArray<FHeartNodeLayer>& Layers)
{
	Layers.Empty();

	TFunction<void(const FHeartTreeNode&, int32)> BuildNodeLayer;
	BuildNodeLayer = [&Layers, &BuildNodeLayer](const FHeartTreeNode& TreeNode, const int32 Depth)
	{
		if (!Layers.IsValidIndex(Depth))
		{
			Layers.SetNum(Depth+1);
		}

		Layers[Depth].Nodes.Add(TreeNode.Node);

		for (auto&& Child : TreeNode.Children)
		{
			BuildNodeLayer(Child.Get<FHeartTreeNode>(), Depth+1);
		}
	};

	BuildNodeLayer(Tree.RootNode, 0);
}