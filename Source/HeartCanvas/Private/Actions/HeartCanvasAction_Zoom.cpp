// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasAction_Zoom.h"

#include "UI/HeartInputActivation.h"
#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasAction_Zoom)

FEventReply UHeartCanvasAction_Zoom::ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	HandleAddZoom(CanvasGraph, Activation);
	return true;
}

FEventReply UHeartCanvasAction_Zoom::ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	HandleAddZoom(CanvasNode->GetCanvas(), Activation);
	return true;
}

FEventReply UHeartCanvasAction_Zoom::ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	HandleAddZoom(CanvasPin->GetCanvas(), Activation);
	return true;
}

void UHeartCanvasAction_Zoom::HandleAddZoom(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation)
{
	if (Activation.EventStruct.GetScriptStruct() == FPointerEvent::StaticStruct())
	{
		CanvasGraph->AddToZoom(Activation.AsPointerEvent().GetWheelDelta(), true);
	}
	else if (Activation.EventStruct.GetScriptStruct() == FHeartManualEvent::StaticStruct())
	{
		CanvasGraph->AddToZoom(Activation.AsManualEvent().EventValue, true);
	}
}