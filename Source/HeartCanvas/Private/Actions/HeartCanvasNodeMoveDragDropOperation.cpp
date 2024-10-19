// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasNodeMoveDragDropOperation.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"

#include "Components/CanvasPanelSlot.h"
#include "ModelView/HeartActionHistory.h"
#include "ModelView/Actions/HeartAction_MoveNodeProxy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasNodeMoveDragDropOperation)

bool UHeartCanvasNodeMoveDragDropOperation::CanRunOnWidget(const UWidget* Widget) const
{
	return Widget && Widget->IsA<UHeartGraphCanvasNode>();
}

bool UHeartCanvasNodeMoveDragDropOperation::SetupDragDropOperation()
{
	if (UHeartGraphCanvasNode* CreatedByNode = Cast<UHeartGraphCanvasNode>(SummonedBy))
	{
		Node = CreatedByNode;
		OriginalLocation = Node->GetGraphNode()->GetLocation();

		if (FSlateApplication::IsInitialized())
		{
			const FVector2f MousePosition = Node->GetCanvas()->GetTickSpaceGeometry()
											.AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());
			const FVector2f NodePosition(Cast<UCanvasPanelSlot>(Node->Slot)->GetPosition());
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

		const FVector2f MousePosition = Canvas->GetTickSpaceGeometry()
											.AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());

		const FVector2f UnclampedPosition = MousePosition + MouseOffset;
		const FVector2f ClampedPosition = ClampToBorder(UnclampedPosition);

		auto&& DIVSettings = Canvas->GetDragIntoViewSettings();

		auto&& Guid = Node->GetGraphNode()->GetGuid();

		if (DIVSettings.EnableDragIntoView)
		{
			FVector2f Diff = ClampedPosition - UnclampedPosition;
			Diff = Diff.ClampAxes(0, DIVSettings.DragIntoViewClamp);
			Diff *= DIVSettings.DragMultiplier;

			Canvas->AddToViewCorner(Diff, DIVSettings.InterpDragIntoView);
			Canvas->SetNodeLocation(Guid, FVector2D(Canvas->UnscalePositionToCanvasZoom_2f(UnclampedPosition)), true);
		}
		else
		{
			Canvas->SetNodeLocation(Guid, FVector2D(Canvas->UnscalePositionToCanvasZoom_2f(ClampedPosition)), true);
		}

		Canvas->GetHeartGraph()->NotifyNodeLocationChanged(Guid, true);
	}
}

void UHeartCanvasNodeMoveDragDropOperation::Drop_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Drop_Implementation(PointerEvent);

	if (!Node.IsValid())
	{
		return;
	}

	const FVector2D NodeLocation = Node->GetGraphNode()->GetLocation();

	if (OriginalLocation.IsSet() &&
		!OriginalLocation.GetValue().Equals(NodeLocation))
	{
		auto&& Canvas = Node->GetCanvas();
		auto&& Guid = Node->GetGraphNode()->GetGuid();

		// Final call to notify graph that the movement in no longer in-progress
		Canvas->GetHeartGraph()->NotifyNodeLocationChanged(Guid, false);

		if (auto&& History = Canvas->GetHeartGraph()->GetExtension<UHeartActionHistory>();
			IsValid(History) && History->GetRecordAllMoves())
		{
			FHeartMoveNodeProxyUndoData LocationData;
			LocationData.Locations.Add(Guid, { OriginalLocation.GetValue(), NodeLocation });

			// Manually record an action
			FHeartActionRecord Record;
			Record.Action = UHeartAction_MoveNodeProxy::StaticClass();
			Record.Arguments.Target = Canvas->GetHeartGraph();
			Record.Arguments.Activation = PointerEvent; // Not used, but for posterity
			Record.UndoData.Add(UHeartAction_MoveNodeProxy::LocationStorage, LocationData);

			History->AddRecord(Record);
		}
	}
}

FVector2f UHeartCanvasNodeMoveDragDropOperation::ClampToBorder(const FVector2f& Value) const
{
	const FVector2f PanelSize = Node->GetCanvas()->GetTickSpaceGeometry().GetLocalSize();
	const FVector2f NodeSize(Node->GetDesiredSize());
	return FVector2f(FMath::Clamp(Value.X, 0.f, (PanelSize - NodeSize).X),
					 FMath::Clamp(Value.Y, 0.f, (PanelSize - NodeSize).Y));
}