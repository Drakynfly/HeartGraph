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

protected:
	virtual FEventReply ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	virtual bool CanUndo(const UObject* Target) const override { return true; }
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) const override;
};