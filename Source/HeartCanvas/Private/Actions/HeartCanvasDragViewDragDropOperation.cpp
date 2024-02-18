// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasDragViewDragDropOperation.h"
#include "UMG/HeartGraphCanvas.h"

#include "Blueprint/SlateBlueprintLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasDragViewDragDropOperation)

bool UHeartCanvasDragViewDragDropOperation::CanRunOnWidget(const UWidget* Widget) const
{
	return Widget && Widget->IsA<UHeartGraphCanvas>();
}

bool UHeartCanvasDragViewDragDropOperation::SetupDragDropOperation()
{
	if (UHeartGraphCanvas* SummonedByAsCanvas = Cast<UHeartGraphCanvas>(SummonedBy))
	{
		Canvas = SummonedByAsCanvas;
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