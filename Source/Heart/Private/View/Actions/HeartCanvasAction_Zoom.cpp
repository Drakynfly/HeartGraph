// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "View/Actions/HeartCanvasAction_Zoom.h"
#include "View/HeartGraphCanvas.h"

void UHeartCanvasAction_Zoom::ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation)
{
	CanvasGraph->AddToZoom(Activation.ActivationValue, true);
}
