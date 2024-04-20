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
		DeltaMousePosition = FSlateApplication::IsInitialized() ? FSlateApplication::Get().GetCursorPos() : FVector2f::ZeroVector;
		return true;
	}

	return false;
}

void UHeartCanvasDragViewDragDropOperation::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);

	if (Canvas.IsValid() && FSlateApplication::IsInitialized())
	{
		const FVector2f MousePosition = FSlateApplication::Get().GetCursorPos();
		const FVector2f ScreenOffset = MousePosition - DeltaMousePosition;

		FVector2D ViewportOffset;
		USlateBlueprintLibrary::ScreenToViewport(Canvas.Get(), FVector2D(ScreenOffset), ViewportOffset);

		Canvas->AddToViewCorner(FVector2f(ViewportOffset) / Canvas->GetZoom(), true);

		DeltaMousePosition = MousePosition;
	}
}