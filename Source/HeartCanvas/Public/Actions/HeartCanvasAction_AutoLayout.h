// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphCanvasAction.h"

#include "HeartCanvasAction_AutoLayout.generated.h"

/**
 *
 */
UCLASS()
class HEARTCANVAS_API UHeartCanvasAction_AutoLayout : public UHeartGraphCanvasAction
{
	GENERATED_BODY()

	virtual FEventReply ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject) override;
};
