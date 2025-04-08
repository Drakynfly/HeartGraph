// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ConnectionDrawingPolicy.h"

namespace Heart::Editor
{
	class FDebugConnectionDrawingPolicy : public FConnectionDrawingPolicy
	{
	public:
		FDebugConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraph);
		UEdGraph* EdGraph;

		virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params) override;
		virtual FVector2D ComputeSplineTangent(const FVector2D& Start, const FVector2D& End) const override;
	};
}
