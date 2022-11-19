// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasAction_Zoom.h"
#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"

void UHeartCanvasAction_Zoom::ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation)
{
	CanvasGraph->AddToZoom(Activation.ActivationValue, true);
}

void UHeartCanvasAction_Zoom::ExecuteOnNode(UHeartGraphCanvasNode* CanvasNode, const FHeartInputActivation& Activation)
{
	CanvasNode->GetCanvas()->AddToZoom(Activation.ActivationValue, true);
}

void UHeartCanvasAction_Zoom::ExecuteOnPin(UHeartGraphCanvasPin* CanvasPin, const FHeartInputActivation& Activation)
{
	CanvasPin->GetCanvas()->AddToZoom(Activation.ActivationValue, true);
}
