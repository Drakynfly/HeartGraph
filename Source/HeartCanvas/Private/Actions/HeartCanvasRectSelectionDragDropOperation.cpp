// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasRectSelectionDragDropOperation.h"

#include "Components/CanvasPanelSlot.h"
#include "UMG/HeartGraphCanvas.h"

bool UHeartCanvasRectSelectionDragDropOperation::SetupDragDropOperation()
{
	Canvas = Cast<UHeartGraphCanvas>(SummonedBy);

	if (!Canvas.IsValid())
	{
		return false;
	}

	if (FSlateApplication::IsInitialized())
	{
		SelectionStart = Canvas->GetTickSpaceGeometry()
								.AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());
	}

	return true;
}

void UHeartCanvasRectSelectionDragDropOperation::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);

	const FVector2D CurrentLocation = Canvas->GetTickSpaceGeometry()
					.AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());


	// Update visual

	if (auto&& Slot = Cast<UCanvasPanelSlot>(DefaultDragVisual->Slot))
	{

	}


	// Update node selection

	if (auto&& Graph = Cast<UHeartGraphCanvas>(SummonedBy))
	{

	}
}
