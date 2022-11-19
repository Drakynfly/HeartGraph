// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasDragViewDragDropOperation.h"
#include "UMG/HeartGraphCanvas.h"

#include "Blueprint/SlateBlueprintLibrary.h"

bool UHeartCanvasDragViewDragDropOperation::SetupDragDropOperation()
{
	if (UHeartGraphCanvas* PayloadAsCanvas = Cast<UHeartGraphCanvas>(Payload))
	{
		Canvas = PayloadAsCanvas;
		DeltaMousePosition = FSlateApplication::IsInitialized() ? FSlateApplication::Get().GetCursorPos() : FVector2D();
		return true;
	}

	return false;
}

void UHeartCanvasDragViewDragDropOperation::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);

	if (Canvas.IsValid() && FSlateApplication::IsInitialized())
	{
		const FVector2D MousePosition = FSlateApplication::Get().GetCursorPos();
		const FVector2D ScreenOffset = MousePosition - DeltaMousePosition;

		FVector2D ViewportOffset;
		USlateBlueprintLibrary::ScreenToViewport(Canvas.Get(), ScreenOffset, ViewportOffset);

		Canvas->AddToViewCorner(ViewportOffset / Canvas->GetZoom(), true);

		DeltaMousePosition = MousePosition;
	}
}
