// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasNodeMoveDragDropOperation.h"

#include "ModelView/HeartGraphNode.h"

#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"

#include "Components/CanvasPanelSlot.h"
#include "General/HeartGeneralUtils.h"

bool UHeartCanvasNodeMoveDragDropOperation::SetupDragDropOperation()
{
	if (UHeartGraphCanvasNode* PayloadAsNode = Cast<UHeartGraphCanvasNode>(Payload))
	{
		Node = PayloadAsNode;

		if (FSlateApplication::IsInitialized())
		{
			const FVector2D MousePosition = Node->GetCanvas()->GetTickSpaceGeometry()
											.AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());
			const FVector2D NodePosition = Cast<UCanvasPanelSlot>(Node->Slot)->GetPosition();
			MouseOffset = NodePosition - MousePosition;
		}

		return true;
	}

	return false;
}

void UHeartCanvasNodeMoveDragDropOperation::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);

	if (Node.IsValid() && FSlateApplication::IsInitialized())
	{
		auto&& Canvas = Node->GetCanvas();

		const FVector2D MousePosition = Canvas->GetTickSpaceGeometry()
											.AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());

		const FVector2D UnclampedPosition = MousePosition + MouseOffset;
		const FVector2D ClampedPosition = ClampToBorder(UnclampedPosition);

		auto&& DIVSettings = Canvas->GetDragIntoViewSettings();

		if (DIVSettings.EnableDragIntoView)
		{
			FVector2D Diff = ClampedPosition - UnclampedPosition;
			Diff = Diff.ClampAxes(0, DIVSettings.DragIntoViewClamp);
			Diff *= DIVSettings.DragMultiplier;

			Canvas->AddToViewCorner(Diff, DIVSettings.InterpDragIntoView);
			Node->GetNode()->SetLocation(Canvas->UnscalePositionToCanvasZoom(UnclampedPosition));
		}
		else
		{
			Node->GetNode()->SetLocation(Canvas->UnscalePositionToCanvasZoom(ClampedPosition));
		}
	}
}

FVector2D UHeartCanvasNodeMoveDragDropOperation::ClampToBorder(const FVector2D& Value) const
{
	const FVector2D PanelSize = Node->GetCanvas()->GetTickSpaceGeometry().GetLocalSize();
	const FVector2D NodeSize = Node->GetDesiredSize();
	return UHeartGeneralUtils::BP_ClampVector2D(Value, FVector2DBounds(FVector2D(0.f), PanelSize - NodeSize));
}
