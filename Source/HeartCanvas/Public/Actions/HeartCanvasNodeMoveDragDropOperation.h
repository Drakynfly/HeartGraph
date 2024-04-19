// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartCanvasDragDropOperation.h"
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
	FVector2f ClampToBorder(const FVector2f& Value) const;

private:
	TWeakObjectPtr<class UHeartGraphCanvasNode> Node;
	TOptional<FVector2D> OriginalLocation;

	// Offset between node location and mouse. This is the point on the widget we clicked on, relative to the widget's top left.
	FVector2f MouseOffset = FVector2f::ZeroVector;
};