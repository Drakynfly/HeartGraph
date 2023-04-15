// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

namespace Nodesoup
{
	/** Distribute vertices equally on a 1.0 radius circle */
	void Circle(TArray<FVector2D>& Positions);

	/** Center and scale vertices so the graph fits on a canvas of given dimensions */
	void CenterAndScale(uint32 Width, uint32 Height, TArray<FVector2D>& Positions);
}
