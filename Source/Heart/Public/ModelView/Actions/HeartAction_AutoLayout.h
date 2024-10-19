// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ModelView/Actions/HeartGraphAction.h"
#include "HeartAction_AutoLayout.generated.h"

/**
 *
 */
UCLASS()
class HEART_API UHeartAction_AutoLayout : public UHeartGraphAction
{
	GENERATED_BODY()

protected:
	virtual bool CanExecute(const UObject* Target) const override;
	virtual FHeartEvent ExecuteOnGraph(IHeartGraphInterface* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const override;
	virtual bool CanUndo(const UObject* Target) const override { return true; }
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) const override;
};