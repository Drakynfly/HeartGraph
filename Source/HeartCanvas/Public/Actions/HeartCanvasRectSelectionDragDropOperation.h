// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartCanvasDragDropOperation.h"
#include "HeartCanvasRectSelectionDragDropOperation.generated.h"

/**
 *
 */
UCLASS()
class HEARTCANVAS_API UHeartCanvasRectSelectionDragDropOperation : public UHeartCanvasDragDropOperation
{
	GENERATED_BODY()

public:
	virtual bool SetupDragDropOperation() override;

	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;

protected:
	TWeakObjectPtr<UHeartGraphCanvas> Canvas;

	// This is the point on the canvas initially dragged from.
	FVector2D SelectionStart = FVector2D::ZeroVector;
};
