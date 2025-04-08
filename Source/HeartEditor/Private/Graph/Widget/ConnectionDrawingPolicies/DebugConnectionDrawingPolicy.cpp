// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "DebugConnectionDrawingPolicy.h"

namespace Heart::Editor
{
	FDebugConnectionDrawingPolicy::FDebugConnectionDrawingPolicy(const int32 InBackLayerID, const int32 InFrontLayerID,
		const float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraph)
	  : FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements),
		EdGraph(InGraph)
	{
	}

	void FDebugConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin,
		FConnectionParams& Params)
	{
		FConnectionDrawingPolicy::DetermineWiringStyle(OutputPin, InputPin, Params);
		Params.WireColor = FColor::Red;
	}

	FVector2D FDebugConnectionDrawingPolicy::ComputeSplineTangent(const FVector2D& Start, const FVector2D& End) const
	{
		const FVector2D Delta = End - Start;
		const FVector2D NormDelta = Delta.GetSafeNormal();

		return NormDelta;
	}
}

