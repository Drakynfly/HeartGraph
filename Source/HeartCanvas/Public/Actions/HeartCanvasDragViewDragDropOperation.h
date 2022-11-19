// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartCanvasDragDropOperation.h"
#include "HeartCanvasDragViewDragDropOperation.generated.h"

/**
 *
 */
UCLASS()
class HEARTCANVAS_API UHeartCanvasDragViewDragDropOperation : public UHeartCanvasDragDropOperation
{
	GENERATED_BODY()

public:
	virtual bool SetupDragDropOperation() override;

	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;

private:
	TWeakObjectPtr<class UHeartGraphCanvas> Canvas;

	// Mouse position last frame
	FVector2D DeltaMousePosition = FVector2D::ZeroVector;
};
