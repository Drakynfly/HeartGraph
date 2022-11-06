// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphCanvasAction.h"
#include "HeartCanvasAction_Zoom.generated.h"

/**
 *
 */
UCLASS()
class HEART_API UHeartCanvasAction_Zoom : public UHeartGraphCanvasAction
{
	GENERATED_BODY()

public:
	virtual void ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation) override;
};
