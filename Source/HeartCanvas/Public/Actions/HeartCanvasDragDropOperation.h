// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Move_To_UMG/HeartDragDropOperation.h"
#include "Components/Widget.h"
#include "HeartCanvasDragDropOperation.generated.h"

class UHeartGraphCanvas;
class UHeartGraphCanvasNode;
class UHeartGraphCanvasPin;

/**
 *
 */
UCLASS(Abstract)
class HEARTCANVAS_API UHeartCanvasDragDropOperation : public UHeartDragDropOperation
{
	GENERATED_BODY()

public:
	virtual bool OnHoverWidget(UWidget* Widget) override;

protected:
	UWidget* GetHoveredWidget() const { return LastHovered.IsValid() ? LastHovered.Get() : nullptr; }

	virtual bool OnHoverGraph(UHeartGraphCanvas* Canvas) { return false; }
	virtual bool OnHoverNode(UHeartGraphCanvasNode* CanvasNode) { return false; }
	virtual bool OnHoverPin(UHeartGraphCanvasPin* CanvasPin) { return false; }
	virtual void OnHoverCleared() {}

	// Is the OnHoverWidget function allowed to cache the result when called on repeat widget.
	UPROPERTY(EditAnywhere, Category = "DragDropOperation")
	bool CacheOnHoverWidgetResult = true;

private:
	TWeakObjectPtr<UWidget> LastHovered;
	bool LastHoveredResult = false;
};