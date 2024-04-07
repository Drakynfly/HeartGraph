// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartCanvasDragDropOperation.h"
#include "ModelView/HeartGraphSchema.h"
#include "HeartPinConnectionDragDropOperation.generated.h"

UCLASS()
class HEARTCANVAS_API UHeartPinConnectionDragDropOperation : public UHeartCanvasDragDropOperation
{
	GENERATED_BODY()

public:
	virtual bool CanRunOnWidget(const UWidget* Widget) const override;
	virtual bool SetupDragDropOperation() override;
	virtual bool OnDropOnWidget(UWidget* Widget) override;

	virtual void Drop_Implementation(const FPointerEvent& PointerEvent) override;
	virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent) override;
	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;

	virtual bool OnHoverPin(UHeartGraphCanvasPin* CanvasPin) override;
	virtual void OnHoverCleared() override;

protected:
	UPROPERTY()
	TWeakObjectPtr<UHeartGraphCanvas> Canvas;

	UPROPERTY()
	TWeakObjectPtr<UHeartGraphCanvasPin> DraggedPin;

	UPROPERTY()
	TWeakObjectPtr<UHeartGraphCanvasPin> HoveredPin;

	FHeartConnectPinsResponse Response;
};