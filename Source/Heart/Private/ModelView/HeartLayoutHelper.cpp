// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartLayoutHelper.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartLayoutHelper)

bool UHeartLayoutHelper::Layout(IHeartGraphInterface* Interface)
{
	const UHeartGraph* Graph = Interface->GetHeartGraph();
	if (IsValid(Graph))
	{
		TArray<FHeartNodeGuid> AllNodes;
		Graph->GetNodeGuids(AllNodes);
		return Layout(Interface, AllNodes);
	}

	return false;
}

bool UHeartLayoutHelper::Layout(IHeartGraphInterface* Interface, const float DeltaTime)
{
	const UHeartGraph* Graph = Interface->GetHeartGraph();
	if (IsValid(Graph))
	{
		TArray<FHeartNodeGuid> AllNodes;
		Graph->GetNodeGuids(AllNodes);
		return Layout(Interface, AllNodes, DeltaTime);
	}

	return false;
}

FHeartGraphAdjacencyList UHeartLayoutHelper::GetGraphAdjacencyList(const IHeartGraphInterface* Interface, const TArray<FHeartNodeGuid>& Nodes)
{
	FHeartGraphAdjacencyList Result;

	const UHeartGraph* Graph = Interface->GetHeartGraph();

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

void UHeartLayoutHelper::ApplyNewPositions(const TScriptInterface<IHeartGraphInterface>& Interface,
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
    Interface->GetHeartGraph()->NotifyNodeLocationsChanged(Touched, TempInProgressToPreventNetReplication);
}

bool UHeartLayoutHelper_BlueprintBase::Layout(IHeartGraphInterface* Interface,
											  const TArray<FHeartNodeGuid>& Nodes)
{
	return Layout_BP(Cast<UObject>(Interface), Nodes);
}