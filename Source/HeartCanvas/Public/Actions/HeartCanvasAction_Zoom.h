// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphCanvasAction.h"
#include "HeartCanvasAction_Zoom.generated.h"

/**
 *
 */
UCLASS()
class HEARTCANVAS_API UHeartCanvasAction_Zoom : public UHeartGraphCanvasAction
{
	GENERATED_BODY()

public:
	virtual void ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual void ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual void ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation, UObject* ContextObject) override;
};
