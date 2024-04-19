// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasAction_Zoom.h"

#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasAction_Zoom)

FReply UHeartCanvasAction_Zoom::ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	HandleAddZoom(CanvasGraph, Activation);
	return FReply::Handled();
}

FReply UHeartCanvasAction_Zoom::ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	HandleAddZoom(CanvasNode->GetCanvas(), Activation);
	return FReply::Handled();
}

FReply UHeartCanvasAction_Zoom::ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	HandleAddZoom(CanvasPin->GetCanvas(), Activation);
	return FReply::Handled();
}

void UHeartCanvasAction_Zoom::HandleAddZoom(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation)
{
	if (const TOptional<FPointerEvent> PointerEvent = Activation.As<FPointerEvent>();
		PointerEvent.IsSet())
	{
		CanvasGraph->AddToZoom(PointerEvent.GetValue().GetWheelDelta(), true);
	}
	else if (const TOptional<FHeartManualEvent> ManualEvent = Activation.As<FHeartManualEvent>();
			 ManualEvent.IsSet())
	{
		CanvasGraph->AddToZoom(static_cast<float>(ManualEvent.GetValue().EventValue), true);
	}
}