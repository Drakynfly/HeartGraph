// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Layouts/HeartLayout_TestSpinner.h"
#include "Model/HeartGraphInterface.h"
#include "Model/HeartGuids.h"

bool UHeartLayout_TestSpinner::Layout(IHeartGraphInterface* Interface,
									  const TArray<FHeartNodeGuid>& Nodes, const float DeltaTime)
{
	TArray<FVector2D> Positions;
	Positions.Reserve(Nodes.Num());
	Algo::Transform(Nodes, Positions,
		[Interface](const FHeartNodeGuid& Node)
		{
			return Interface->GetNodeLocation(Node);
		});

	for (auto& Position : Positions)
	{
		Position = Position.GetRotated(360.0 * DeltaTime);
	}

	ApplyNewPositions(Interface->_getUObject(), Nodes, Positions);

	return true;
}