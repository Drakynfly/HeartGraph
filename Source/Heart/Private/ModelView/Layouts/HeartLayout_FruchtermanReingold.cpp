// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Layouts/HeartLayout_FruchtermanReingold.h"
#include "Model/HeartGraphInterface.h"
#include "Model/HeartGuids.h"
#include "Algorithms/FruchtermanReingold.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartLayout_FruchtermanReingold)

bool UHeartLayout_FruchtermanReingold::Layout(IHeartGraphInterface* Interface,
											  const TArray<FHeartNodeGuid>& Nodes, const float DeltaTime)
{
	TArray<FVector2D> Positions;
	Positions.Reserve(Nodes.Num());
	Algo::Transform(Nodes, Positions,
		[Interface](const FHeartNodeGuid& Node)
		{
			return Interface->GetNodeLocation(Node);
		});

	//if (!Algorithm.IsSet())
	{
		// @todo rebuild this when graph changes!
		AdjacencyList = GetGraphAdjacencyList(Interface, Nodes);
		Algorithm = Nodesoup::FruchtermanReingold(AdjacencyList.AdjacencyList, Strength);
	}

	Accum += DeltaTime;
	const float IterationInterval = 1.f / IterationsPerSecond;
	while (Accum > IterationInterval)
	{
		Algorithm.GetValue()(Positions);
		Accum -= IterationInterval;
	}

	ApplyNewPositions(Cast<UObject>(Interface), Nodes, Positions);

	return true;
}