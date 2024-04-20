// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartActionBase.h"
#include "HeartAction_MultiUndoEnd.generated.h"

/**
 * This action class is a scope marker for collapsing multiple actions into a single undo/redo group.
 * This the is "End" scope, and is only ever ran as an Undo.
 */
UCLASS(MinimalAPI)
class UHeartAction_MultiUndoEnd final : public UHeartActionBase
{
	GENERATED_BODY()

protected:
	virtual FText GetDescription(const UObject* Target) const override;
	virtual bool CanExecute(const UObject* Target) const override { return false; }
	virtual bool CanUndo(const UObject* Target) const override { return true; }
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) const override;
};