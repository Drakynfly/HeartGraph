// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartCanvasDragDropOperation.h"
#include "HeartCanvasActionDragDropOperation.generated.h"

/**
 * A drag drop operation that carries a UHeartGraphCanvasAction
 */
UCLASS()
class HEARTCANVAS_API UHeartCanvasActionDragDropOperation : public UHeartCanvasDragDropOperation
{
	GENERATED_BODY()

public:
	virtual void Drop_Implementation(const FPointerEvent& PointerEvent) override;
};
