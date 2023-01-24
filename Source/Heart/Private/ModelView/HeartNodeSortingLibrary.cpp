// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartNodeSortingLibrary.h"
#include "Model/HeartGraphNode.h"

void UHeartNodeSortingLibrary::SortLooseNodesIntoTrees(const TArray<UHeartGraphNode*>& Nodes, const FNodeLooseToTreeArgs& Args, TArray<FHeartTree>& Trees)
{
	// Find all nodes that have no pins connections in the specified direction
	TArray<UHeartGraphNode*> RootNodes =  Nodes.FilterByPredicate([&Args](const UHeartGraphNode* Node)
	{
		return Node->CountPinsByPredicate(Args.Direction, [](const UHeartGraphPin* Pin)
		{
			return Pin->IsConnected();
		}) == 0;
	});

	EHeartPinDirection InverseDirection = Args.Direction == EHeartPinDirection::Input ? EHeartPinDirection::Output : EHeartPinDirection::Input;

	// Recursive function for building tree nodes
	TFunction<FHeartTreeNode(UHeartGraphNode*)> BuildTreeNode;
	BuildTreeNode = [InverseDirection, &BuildTreeNode](UHeartGraphNode* Node)
	{
		FHeartTreeNode OutTreeNode;
		OutTreeNode.Node = Node->GetGuid();
		auto&& Pins = Node->GetPinsOfDirection(InverseDirection);
		for (auto&& Pin : Pins)
		{
			auto&& ConnectedPins = Pin->GetAllConnections();
			for (auto&& ConnectedPin : ConnectedPins)
			{
				if (auto&& ConnectedNode = ConnectedPin->GetNode())
				{
					OutTreeNode.Children.Add(FInstancedStruct::Make(BuildTreeNode(ConnectedNode)));
				}
			}
		}

		return OutTreeNode;
	};

	for (auto&& RootNode : RootNodes)
	{
		Trees.Add({BuildTreeNode(RootNode)});
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
