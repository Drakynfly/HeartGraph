// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasDragDropOperation.h"
#include "HeartCanvasLog.h"
#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasDragDropOperation)

bool UHeartCanvasDragDropOperation::OnHoverWidget(UWidget* Widget)
{
	// Return false if there is nothing hovered
	if (!LastHovered.IsValid() && Widget == nullptr)
	{
		return false;
	}

	// Return cached value when called on repeat widget. This avoids calling OnHover functions again
	if (Widget == LastHovered && CacheOnHoverWidgetResult)
	{
		return LastHoveredResult;
	}

	UE_LOG(LogHeartCanvas, Log, TEXT("OnHoverWidget changed: %s"), Widget ? *Widget->GetName() : TEXT("nullptr"))

	// If there could be a current hover than clear it
	if (!LastHovered.IsExplicitlyNull())
	{
		// Clear existing hover
		OnHoverCleared();
		LastHovered = nullptr;
	}

	if (Widget)
	{
		// Record this new hover
		LastHovered = Widget;

		// Try supported classes:
		bool Result = false;

		if (UHeartGraphCanvas* Canvas = Cast<UHeartGraphCanvas>(Widget))
		{
			Result = OnHoverGraph(Canvas);
		}
		else if (UHeartGraphCanvasNode* CanvasNode = Cast<UHeartGraphCanvasNode>(Widget))
		{
			Result = OnHoverNode(CanvasNode);
		}
		else if (UHeartGraphCanvasPin* CanvasPin = Cast<UHeartGraphCanvasPin>(Widget))
		{
			Result = OnHoverPin(CanvasPin);
		}

		LastHoveredResult = Result;
		return Result;
	}

	return false;
}