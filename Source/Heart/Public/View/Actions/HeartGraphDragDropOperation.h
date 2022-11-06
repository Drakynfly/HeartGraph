// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UI/HeartDragDropOperation.h"
#include "HeartGraphDragDropOperation.generated.h"

class UHeartGraphCanvas;
class UHeartGraphCanvasNode;
class UHeartGraphCanvasPin;

/**
 *
 */
UCLASS(Abstract)
class HEART_API UHeartGraphDragDropOperation : public UHeartDragDropOperation
{
	GENERATED_BODY()

public:
	virtual bool OnHoverWidget(UWidget* Widget) override;

protected:
	virtual bool OnHoverGraph(UHeartGraphCanvas* Canvas) { return false; }
	virtual bool OnHoverNode(UHeartGraphCanvasNode* CanvasNode) { return false; }
	virtual bool OnHoverPin(UHeartGraphCanvasPin* CanvasPin) { return false; }
	virtual void OnHoverCleared() {}

private:
	TWeakObjectPtr<UWidget> LastHovered;
};
