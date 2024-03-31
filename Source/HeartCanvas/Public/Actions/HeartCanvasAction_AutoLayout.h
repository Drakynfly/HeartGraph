// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphCanvasAction.h"
#include "Model/HeartGuids.h"

#include "HeartCanvasAction_AutoLayout.generated.h"

/**
 *
 */
UCLASS()
class HEARTCANVAS_API UHeartCanvasAction_AutoLayout : public UHeartGraphCanvasAction
{
	GENERATED_BODY()

public:
	virtual FEventReply ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject) override;
	virtual bool CanUndo(UObject* Target) const override { return true; }
	virtual bool Undo(UObject* Target) override;

private:
	// Undo data
	TMap<FHeartNodeGuid, FVector2D> OriginalLocations;
};