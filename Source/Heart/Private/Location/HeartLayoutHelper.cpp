// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Location/HeartLayoutHelper.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartLayoutHelper)

bool UHeartLayoutHelper::Layout(UHeartGraph& Graph, IHeartNodeLocationInterface& Interface)
{
	TArray<FHeartNodeGuid> AllNodes;
	Graph.GetNodeGuids(AllNodes);
	return Layout(Graph, Interface, AllNodes);
}

bool UHeartLayoutHelper::Layout(UHeartGraph& Graph, IHeartNodeLocationInterface& Interface, const float DeltaTime)
{
	TArray<FHeartNodeGuid> AllNodes;
	Graph.GetNodeGuids(AllNodes);
	return Layout(Graph, Interface, AllNodes, DeltaTime);
}

FHeartGraphAdjacencyList UHeartLayoutHelper::GetGraphAdjacencyList(const UHeartGraph& Graph, const TArray<FHeartNodeGuid>& Nodes)
{
	FHeartGraphAdjacencyList Result;

	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		const TArray<FHeartNodeGuid> OutputLinks = Heart::Utils::GetConnectedNodes(Graph, Nodes[i], EHeartPinDirection::Output);

		TArray<int32>& NodeAdjacency = Result.AdjacencyList.AddDefaulted_GetRef();
		NodeAdjacency.Reserve(OutputLinks.Num());

		for (auto&& OutputLink : OutputLinks)
		{
			if (const int32 NodeIndex = Nodes.Find(OutputLink);
				NodeIndex != INDEX_NONE)
			{
				NodeAdjacency.Add(NodeIndex);
			}
		}
	}

	return Result;
}

void UHeartLayoutHelper::ApplyNewPositions(const TScriptInterface<IHeartNodeLocationInterface>& Interface,
										   const TArray<FHeartNodeGuid>& Nodes,
										   const TArray<FVector2D>& NewPositions) const
{
    for (int32 i = 0; i < NewPositions.Num(); ++i)
    {
    	Interface->SetNodeLocation(Nodes[i], NewPositions[i], false);
    }

	// @todo fix
	static constexpr bool TempInProgressToPreventNetReplication = true;

	TSet<FHeartNodeGuid> Touched;
	Touched.Append(Nodes);
    Interface->NotifyNodeLocationsChanged(Touched, TempInProgressToPreventNetReplication);
}

bool UHeartLayoutHelper_BlueprintBase::Layout(UHeartGraph& Graph, IHeartNodeLocationInterface& Interface,
											  const TArray<FHeartNodeGuid>& Nodes)
{
	return Layout_BP(&Graph, Cast<UObject>(&Interface), Nodes);
}