// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "View/Actions/HeartGraphDragDropOperation.h"
#include "View/HeartGraphCanvas.h"
#include "View/HeartGraphCanvasNode.h"
#include "View/HeartGraphCanvasPin.h"

bool UHeartGraphDragDropOperation::OnHoverWidget(UWidget* Widget)
{
	if (LastHovered.IsValid())
	{
		if (Widget == LastHovered)
		{
			return false;
		}
	}

	OnHoverCleared();

	if (UHeartGraphCanvas* Canvas = Cast<UHeartGraphCanvas>(Widget))
	{
		return OnHoverGraph(Canvas);
	}

	if (UHeartGraphCanvasNode* CanvasNode = Cast<UHeartGraphCanvasNode>(Widget))
	{
		return OnHoverNode(CanvasNode);
	}

	if (UHeartGraphCanvasPin* CanvasPin = Cast<UHeartGraphCanvasPin>(Widget))
	{
		return OnHoverPin(CanvasPin);
	}

	LastHovered = Widget;

	return false;
}
