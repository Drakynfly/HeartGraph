// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Blueprint/DragDropOperation.h"
#include "HeartDragDropOperation.generated.h"

class UHeartGraphCanvas;

/**
 *
 */
UCLASS(Abstract)
class HEARTCORE_API UHeartDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	virtual bool SetupDragDropOperation() { return false; }

	virtual bool CanDropOnWidget(UWidget* Widget) { return false; }

	virtual bool OnHoverWidget(UWidget* Widget) { return false; }
};
