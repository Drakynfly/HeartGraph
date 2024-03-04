﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartNodeSortingLibrary.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartQueries.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNodeSortingLibrary)

TArray<UHeartGraphNode*> UHeartNodeSortingLibrary::SortNodes(const TArray<UHeartGraphNode*>& Nodes,
	const FHeartNodeComparePredicate& Predicate)
{
	if (!ensure(Predicate.IsBound()))
	{
		return TArray<UHeartGraphNode*>();
	}

	TArray<UHeartGraphNode*> OutNodes = Nodes;
	SortNodesInPlace(OutNodes, Predicate);
	return OutNodes;
}

void UHeartNodeSortingLibrary::SortNodesInPlace(TArray<UHeartGraphNode*>& Nodes, const FHeartNodeComparePredicate& Predicate)
{
	if (!ensure(Predicate.IsBound()))
	{
		return;
	}

	Algo::Sort(Nodes, [&Predicate](const UHeartGraphNode* A, const UHeartGraphNode* B)
		{
			return Predicate.Execute(A, B);
		});
}

TArray<UHeartGraphNode*> UHeartNodeSortingLibrary::FilterNodesByPredicate(const TArray<UHeartGraphNode*>& Nodes,
                                                                          const FHeartNodeFilterPredicate& Predicate)
{
	if (!ensure(Predicate.IsBound()))
	{
		return TArray<UHeartGraphNode*>();
	}

	return Nodes.FilterByPredicate([&Predicate](const UHeartGraphNode* Node)
		{
			return Predicate.Execute(Node);
		});
}

TArray<UHeartGraphNode*> UHeartNodeSortingLibrary::FilterNodesByPredicate_Exclusive(
	const TArray<UHeartGraphNode*>& Nodes, const FHeartNodeFilterPredicate& Predicate)
{
	if (!ensure(Predicate.IsBound()))
	{
		return TArray<UHeartGraphNode*>();
	}

	return Nodes.FilterByPredicate([&Predicate](const UHeartGraphNode* Node)
		{
			return !Predicate.Execute(Node);
		});
}

TArray<UHeartGraphNode*> UHeartNodeSortingLibrary::FilterNodesByClass(const TArray<UHeartGraphNode*>& Nodes,
                                                                      const TSet<TSubclassOf<UHeartGraphNode>>& Classes)
{
	if (!ensure(!Classes.IsEmpty()))
	{
		return TArray<UHeartGraphNode*>();
	}

	return Nodes.FilterByPredicate([&Classes](const UHeartGraphNode* Node)
	{
		for (auto&& Class = Node->GetClass(); Class && Class != UObject::StaticClass(); Class = Class->GetSuperClass())
		{
			if (Classes.Contains(Class))
			{
				return true;
			}
		}
		return false;
	});
}

TArray<UHeartGraphNode*> UHeartNodeSortingLibrary::FilterNodesByClass_Exclusive(const TArray<UHeartGraphNode*>& Nodes,
                                                                                const TSet<TSubclassOf<UHeartGraphNode>>& Classes)
{
	if (!ensure(!Classes.IsEmpty()))
	{
		return TArray<UHeartGraphNode*>();
	}

	return Nodes.FilterByPredicate([&Classes](const UHeartGraphNode* Node)
		{
			for (auto&& Class = Node->GetClass(); Class && Class != UObject::StaticClass(); Class = Class->GetSuperClass())
			{
				if (Classes.Contains(Class))
				{
					return false;
				}
			}
			return true;
		});
}

void UHeartNodeSortingLibrary::SortLooseNodesIntoTrees(const TArray<UHeartGraphNode*>& Nodes, const FNodeLooseToTreeArgs& Args, TArray<FHeartTree>& Trees)
{
	// Find all nodes that have no pins connections in the specified direction
	TArray<UHeartGraphNode*> RootNodes = Nodes.FilterByPredicate([&Args](const UHeartGraphNode* Node)
	{
		return Node->FindPinsByPredicate(Args.Direction,
			[Node](const FHeartPinGuid Pin, const FHeartGraphPinDesc&)
			{
				return Node->HasConnections(Pin);
			}).Get().Num() == 0;
	});

	EHeartPinDirection InverseDirection = Args.Direction == EHeartPinDirection::Input ? EHeartPinDirection::Output : EHeartPinDirection::Input;

	TSet<UHeartGraphNode*> TrackedNodes;

	// Recursive function for building tree nodes
	TFunction<FHeartTreeNode(UHeartGraphNode*)> BuildTreeNode;
	BuildTreeNode = [InverseDirection, &BuildTreeNode, &TrackedNodes, AllowDuplicates = Args.AllowDuplicates](const UHeartGraphNode* Node)
	{
		FHeartTreeNode OutTreeNode;
		OutTreeNode.Node = Node->GetGuid();
		for (auto&& Pins = Node->FindPinsByDirection(InverseDirection).Get();
			auto&& Pin : Pins)
		{
			auto&& Connections = Node->GetLinks(Pin);
			if (!Connections.IsSet())
			{
				continue;
			}

			for (auto&& ConnectedPin : Connections.GetValue().Links)
			{
				if (auto&& ConnectedNode = Node->GetGraph()->GetNode(ConnectedPin.NodeGuid))
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
		}

		return OutTreeNode;
	};

	for (auto&& RootNode : RootNodes)
	{
		Trees.Add({RootNode->GetGraph(), BuildTreeNode(RootNode)});
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