// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartActionBase.h"
#include "HeartAction_MultiUndoStart.generated.h"

/**
 * This action class is a scope marker for collapsing multiple actions into a single undo/redo group.
 * This the is "Start" scope, and is only ever ran as a Redo.
 */
UCLASS(MinimalAPI)
class UHeartAction_MultiUndoStart final : public UHeartActionBase
{
	GENERATED_BODY()

protected:
	virtual FText GetDescription(const UObject* Target) const override;
	virtual bool CanExecute(const UObject* Target) const override { return true; }
	virtual FHeartEvent Execute(const Heart::Action::FArguments& Arguments) const override;
};