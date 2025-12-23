// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Location/Layouts/HeartLayout_TestSpinner.h"
#include "Location/HeartNodeLocationInterface.h"
#include "Model/HeartGuids.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartLayout_TestSpinner)

bool UHeartLayout_TestSpinner::Layout(UHeartGraph& Graph, IHeartNodeLocationInterface& Interface,
									  const TArray<FHeartNodeGuid>& Nodes, const float DeltaTime)
{
	TArray<FVector2D> Positions;
	Positions.Reserve(Nodes.Num());
	Algo::Transform(Nodes, Positions,
		[&Interface](const FHeartNodeGuid& Node)
		{
			return Interface.GetNodeLocation(Node);
		});

	for (auto& Position : Positions)
	{
		Position = Position.GetRotated(360.0 * DeltaTime);
	}

	ApplyNewPositions(Cast<UObject>(&Interface), Nodes, Positions);

	return true;
}