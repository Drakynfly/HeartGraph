// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Layouts/HeartLayout_TestSpinner.h"
#include "Model/HeartGuids.h"
#include "ModelView/HeartNodeLocationAccessor.h"

bool UHeartLayout_TestSpinner::Layout(IHeartNodeLocationAccessor* Accessor,
									  const TArray<FHeartNodeGuid>& Nodes, const float DeltaTime)
{
	TArray<FVector2D> Positions;
	Positions.Reserve(Nodes.Num());
	Algo::Transform(Nodes, Positions,
		[Accessor](const FHeartNodeGuid& Node)
		{
			return Accessor->GetNodeLocation(Node);
		});

	for (auto& Position : Positions)
	{
		Position = Position.GetRotated(360.0 * DeltaTime);
	}

	ApplyNewPositions(Accessor->_getUObject(), Nodes, Positions);

	return true;
}