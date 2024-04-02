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
	virtual FEventReply ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual FEventReply ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;
	virtual FEventReply ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) override;

protected:
	void HandleAddZoom(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation);
};