// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartCanvasDragDropOperation.h"
#include "HeartCanvasNodeMoveDragDropOperation.generated.h"

/**
 *
 */
UCLASS()
class HEARTCANVAS_API UHeartCanvasNodeMoveDragDropOperation : public UHeartCanvasDragDropOperation
{
	GENERATED_BODY()

public:
	virtual bool CanRunOnWidget(const UWidget* Widget) const override;

	virtual bool SetupDragDropOperation() override;

	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;

	virtual void Drop_Implementation(const FPointerEvent& PointerEvent) override;

protected:
	FVector2D ClampToBorder(const FVector2D& Value) const;

private:
	TWeakObjectPtr<class UHeartGraphCanvasNode> Node;

	// Offset between node location and mouse. This is the point on the widget we clicked on, relative to the widget's top left.
	FVector2D MouseOffset = FVector2D::ZeroVector;
};
