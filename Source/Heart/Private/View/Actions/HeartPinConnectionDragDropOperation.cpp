// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "View/Actions/HeartPinConnectionDragDropOperation.h"
#include "View/Actions/PinConnectionStatusInterface.h"
#include "View/HeartGraphCanvas.h"
#include "View/HeartGraphCanvasPin.h"
#include "Model/HeartGraph.h"
#include "ModelView/HeartGraphSchema.h"

bool UHeartPinConnectionDragDropOperation::SetupDragDropOperation()
{
	if (UHeartGraphCanvasPin* CanvasPin = Cast<UHeartGraphCanvasPin>(Payload))
	{
		DraggedPin = CanvasPin;
		Canvas = CanvasPin->GetCanvas();
		if (Canvas.IsValid())
		{
			Canvas->SetPreviewConnection(DraggedPin->GetPin()->GetReference());
			DraggedPin->SetIsPreviewConnectionTarget(true, true);
			return true;
		}
	}

	return false;
}

bool UHeartPinConnectionDragDropOperation::CanDropOnWidget(UWidget* Widget)
{
	return IsValid(Cast<UHeartGraphCanvasPin>(Widget));
}

void UHeartPinConnectionDragDropOperation::Drop_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Drop_Implementation(PointerEvent);

	if (DraggedPin.IsValid() && HoveredPin.IsValid())
	{
		if (Canvas->GetGraph()->GetSchema()->TryConnectPins(DraggedPin->GetPin(), HoveredPin->GetPin()))
		{
		}

		DraggedPin->SetIsPreviewConnectionTarget(false, false);
		HoveredPin->SetIsPreviewConnectionTarget(false, false);
	}


	Canvas->SetPreviewConnection(FHeartGraphPinReference());
}

void UHeartPinConnectionDragDropOperation::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
	Super::DragCancelled_Implementation(PointerEvent);

	if (DraggedPin.IsValid())
	{
		DraggedPin->SetIsPreviewConnectionTarget(false, false);
	}

	if (HoveredPin.IsValid())
	{
		HoveredPin->SetIsPreviewConnectionTarget(false, false);
	}

	Canvas->SetPreviewConnection(FHeartGraphPinReference());
}

void UHeartPinConnectionDragDropOperation::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);
}

bool UHeartPinConnectionDragDropOperation::OnHoverPin(UHeartGraphCanvasPin* CanvasPin)
{
	bool HandledHover = false;

	if (CanvasPin && CanvasPin != HoveredPin)
	{
		if (HoveredPin.IsValid())
		{
			HoveredPin->SetIsPreviewConnectionTarget(false, false);
		}

		HoveredPin = CanvasPin;
		Response = Canvas->GetGraph()->GetSchema()->CanPinsConnect(DraggedPin->GetPin(), HoveredPin->GetPin());
		HoveredPin->SetIsPreviewConnectionTarget(true, Response.Response != EHeartCanConnectPinsResponse::Disallow);
		HandledHover = true;
	}
	else
	{
		HoveredPin = nullptr;
		Response = FHeartConnectPinsResponse();
	}

	if (DefaultDragVisual && DefaultDragVisual->Implements<UPinConnectionStatusInterface>())
	{
		IPinConnectionStatusInterface::Execute_SetConnectionResponse(DefaultDragVisual, Response);
	}

	return HandledHover;
}

void UHeartPinConnectionDragDropOperation::OnHoverCleared()
{
	if (HoveredPin.IsValid())
	{
		HoveredPin->SetIsPreviewConnectionTarget(false, false);
	}

	HoveredPin = nullptr;
	Response = FHeartConnectPinsResponse();

	if (DefaultDragVisual && DefaultDragVisual->Implements<UPinConnectionStatusInterface>())
	{
		IPinConnectionStatusInterface::Execute_SetConnectionResponse(DefaultDragVisual, Response);
	}
}
