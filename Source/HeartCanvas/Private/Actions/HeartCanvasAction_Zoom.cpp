// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasAction_Zoom.h"
#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"

FEventReply UHeartCanvasAction_Zoom::ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	CanvasGraph->AddToZoom(Activation.ActivationValue, true);
	return true;
}

FEventReply UHeartCanvasAction_Zoom::ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	CanvasNode->GetCanvas()->AddToZoom(Activation.ActivationValue, true);
	return true;
}

FEventReply UHeartCanvasAction_Zoom::ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	CanvasPin->GetCanvas()->AddToZoom(Activation.ActivationValue, true);
	return true;
}
