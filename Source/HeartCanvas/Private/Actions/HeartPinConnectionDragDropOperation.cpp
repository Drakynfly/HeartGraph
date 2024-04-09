// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartPinConnectionDragDropOperation.h"
#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasPin.h"

#include "Model/HeartGraph.h"
#include "ModelView/HeartActionHistory.h"
#include "ModelView/HeartGraphSchema.h"
#include "ModelView/Actions/HeartAction_PinsEditProxy.h"
#include "View/PinConnectionStatusInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartPinConnectionDragDropOperation)

bool UHeartPinConnectionDragDropOperation::CanRunOnWidget(const UWidget* Widget) const
{
	return Widget && Widget->IsA<UHeartGraphCanvasPin>();
}

bool UHeartPinConnectionDragDropOperation::SetupDragDropOperation()
{
	if (UHeartGraphCanvasPin* CanvasPin = Cast<UHeartGraphCanvasPin>(SummonedBy))
	{
		DraggedPin = CanvasPin;
		Canvas = CanvasPin->GetCanvas();
		if (Canvas.IsValid())
		{
			Canvas->SetPreviewConnection(DraggedPin->GetPinReference());
			DraggedPin->SetIsPreviewConnectionTarget(true, true);
			return true;
		}
	}

	return false;
}

bool UHeartPinConnectionDragDropOperation::OnDropOnWidget(UWidget* Widget)
{
	return IsValid(Cast<UHeartGraphCanvasPin>(Widget));
}

void UHeartPinConnectionDragDropOperation::Drop_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Drop_Implementation(PointerEvent);

	if (DraggedPin.IsValid() && HoveredPin.IsValid())
	{
		UHeartGraph* Graph = Canvas->GetGraph();
		auto&& FromPin = DraggedPin->GetPinReference();
		auto&& ToPin = HoveredPin->GetPinReference();

		if (auto&& History = Graph->GetExtension<UHeartActionHistory>())
		{
			// Stash the state of the pins prior to running TryConnectPins
			FHeartPinsEditProxyUndoData MementoData;
			Graph->EditConnections()
				.CreateMementos(FromPin, MementoData.Original)
				.CreateMementos(ToPin, MementoData.Original);

			if (Graph->GetSchema()->TryConnectPins(Graph, FromPin, ToPin))
			{
				{
					Heart::Connections::FEdit Edit = Graph->EditConnections();
					for (auto&& Element : MementoData.Original)
					{
						Edit.CreateAllMementos(Element.Key, MementoData.New);
					}
				}

				// Manually record an action, if successfully modified connections
				FHeartActionRecord Record;
				Record.Action = UHeartAction_PinsEditProxy::StaticClass();
				Record.Arguments.Target = Graph;
				Record.Arguments.Activation = PointerEvent; // Not used, but for posterity
				Record.UndoData.Add(UHeartAction_PinsEditProxy::MementosStorage, MementoData);
				History->AddRecord(Record);
			}
		}
		else
		{
			if (Graph->GetSchema()->TryConnectPins(Graph, FromPin, ToPin))
			{
			}
		}
	}

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
	if (ensure(IsValid(CanvasPin)))
	{
		if (CanvasPin != HoveredPin)
		{
			if (HoveredPin.IsValid())
			{
				HoveredPin->SetIsPreviewConnectionTarget(false, false);
			}

			HoveredPin = CanvasPin;
			Response = Canvas->GetGraph()->GetSchema()->CanPinsConnect(Canvas->GetGraph(),
					DraggedPin->GetPinReference(), HoveredPin->GetPinReference());
			HoveredPin->SetIsPreviewConnectionTarget(true, Response.Response != EHeartCanConnectPinsResponse::Disallow);

			if (DefaultDragVisual && DefaultDragVisual->Implements<UPinConnectionStatusInterface>())
			{
				IPinConnectionStatusInterface::Execute_SetConnectionResponse(DefaultDragVisual, Response);
			}
		}

		return true;
	}

	return false;
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