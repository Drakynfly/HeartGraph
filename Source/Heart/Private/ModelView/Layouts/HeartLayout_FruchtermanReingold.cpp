// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Layouts/HeartLayout_FruchtermanReingold.h"
#include "ModelView/HeartNodeLocationAccessor.h"
#include "Algorithms/FruchtermanReingold.h"
#include "Model/HeartGuids.h"

bool UHeartLayout_FruchtermanReingold::Layout(IHeartNodeLocationAccessor* Accessor,
											  const TArray<FHeartNodeGuid>& Nodes, const float DeltaTime)
{
	TArray<FVector2D> Positions;
	Positions.Reserve(Nodes.Num());
	Algo::Transform(Nodes, Positions,
		[Accessor](const FHeartNodeGuid& Node)
		{
			return Accessor->GetNodeLocation(Node);
		});

	//if (!Algorithm.IsSet())
	{
		// @todo rebuild this when graph changes!
		AdjacencyList = GetGraphAdjacencyList(Accessor, Nodes);
		Algorithm = Nodesoup::FruchtermanReingold(AdjacencyList.AdjacencyList, Strength);
	}

	Accum += DeltaTime;
	const float IterationInterval = 1.f / IterationsPerSecond;
	while (Accum > IterationInterval)
	{
		Algorithm.GetValue()(Positions);
		Accum -= IterationInterval;
	}

	ApplyNewPositions(Accessor->_getUObject(), Nodes, Positions);

	return true;
}