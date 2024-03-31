// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Blueprint/DragDropOperation.h"
#include "HeartDragDropOperation.generated.h"

/**
 * A tickable drag drop operation
 */
UCLASS(Abstract)
class HEARTCANVAS_API UHeartDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	virtual bool CanRunOnWidget(const UWidget* Widget) const { return true; }

	virtual bool SetupDragDropOperation() PURE_VIRTUAL(UHeartDragDropOperation::SetupDragDropOperation, return false; )

	virtual bool CanDropOnWidget(UWidget* Widget) { return true; }

	virtual bool OnHoverWidget(UWidget* Widget) { return false; }

	// The widget that created this drag drop operation
	UPROPERTY()
	TObjectPtr<UWidget> SummonedBy;
};