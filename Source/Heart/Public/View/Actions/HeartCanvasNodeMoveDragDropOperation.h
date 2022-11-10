// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphDragDropOperation.h"
#include "HeartCanvasNodeMoveDragDropOperation.generated.h"

/**
 *
 */
UCLASS()
class HEART_API UHeartCanvasNodeMoveDragDropOperation : public UHeartGraphDragDropOperation
{
	GENERATED_BODY()

public:
	virtual bool SetupDragDropOperation() override;

	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;

private:
	TWeakObjectPtr<class UHeartGraphCanvasNode> Node;

	// Mouse position last frame
	FVector2D DeltaMousePosition = FVector2D::ZeroVector;
};
