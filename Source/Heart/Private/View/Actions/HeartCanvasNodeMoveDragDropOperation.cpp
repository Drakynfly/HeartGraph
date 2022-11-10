// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "View/Actions/HeartCanvasNodeMoveDragDropOperation.h"

#include "View/HeartGraphCanvasNode.h"

bool UHeartCanvasNodeMoveDragDropOperation::SetupDragDropOperation()
{
	if (UHeartGraphCanvasNode* PayloadAsNode = Cast<UHeartGraphCanvasNode>(Payload))
	{
		Node = PayloadAsNode;
		DeltaMousePosition = FSlateApplication::IsInitialized() ? FSlateApplication::Get().GetCursorPos() : FVector2D();
		return true;
	}

	return false;
}

void UHeartCanvasNodeMoveDragDropOperation::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);
}
