// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Containers/Array.h"

namespace Nodesoup
{
	/** Distribute vertices equally on a 1.0 radius circle */
	void HEARTCORE_API Circle(TArray<FVector2D>& Positions);

	/** Center and scale vertices so the graph fits on a canvas of given dimensions */
	void HEARTCORE_API CenterAndScale(uint32 Width, uint32 Height, TArray<FVector2D>& Positions);
}
