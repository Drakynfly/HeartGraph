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
			Canvas->SetNodeLocation(Node->GetGraphNode()->GetGuid(), Canvas->UnscalePositionToCanvasZoom(UnclampedPosition), true);
		}
		else
		{
			Canvas->SetNodeLocation(Node->GetGraphNode()->GetGuid(), Canvas->UnscalePositionToCanvasZoom(ClampedPosition), true);
		}

		Canvas->GetHeartGraph()->NotifyNodeLocationsChanged({Node->GetGraphNode()}, true);
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

		// Final call to notify graph that the movement in no longer in-progress
		Canvas->GetHeartGraph()->NotifyNodeLocationsChanged({Node->GetGraphNode()}, false);

		if (auto&& History = Canvas->GetHeartGraph()->GetExtension<UHeartActionHistory>())
		{
			TMap<FHeartNodeGuid, FHeartMoveNodeProxyLocationPair> Locations;
			Locations.Add(Node->GetGraphNode()->GetGuid(),
				{ OriginalLocation.GetValue(), NodeLocation });

			// Manually record an action
			FHeartActionRecord Record;
			Record.Action = UHeartAction_MoveNodeProxy::StaticClass();
			Record.Arguments.Target = Canvas->GetHeartGraph();
			Record.Arguments.Activation = PointerEvent; // Not used, but for posterity
			Record.UndoData.Add(UHeartAction_MoveNodeProxy::LocationStorage, Locations);

			History->AddRecord(Record);
		}
	}
}

FVector2D UHeartCanvasNodeMoveDragDropOperation::ClampToBorder(const FVector2D& Value) const
{
	const FVector2D PanelSize = Node->GetCanvas()->GetTickSpaceGeometry().GetLocalSize();
	const FVector2D NodeSize = Node->GetDesiredSize();
	return FVector2D(FMath::Clamp(Value.X, 0.0, (PanelSize - NodeSize).X),
					 FMath::Clamp(Value.Y, 0.0, (PanelSize - NodeSize).Y));
}