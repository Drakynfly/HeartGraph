// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "View/Actions/HeartCanvasDragViewDragDropOperation.h"
#include "View/HeartGraphCanvas.h"

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

	if (Canvas.IsValid())
	{
		const FVector2D MousePos = FSlateApplication::IsInitialized() ? FSlateApplication::Get().GetCursorPos() : FVector2D();
		const FVector2D ScreenOffset = MousePos - DeltaMousePosition;

		FVector2D ViewportOffset;
		USlateBlueprintLibrary::ScreenToViewport(Canvas.Get(), ScreenOffset, ViewportOffset);

		Canvas->AddToViewCorner(ViewportOffset / Canvas->GetZoom(), true);

		DeltaMousePosition = MousePos;
	}
}
