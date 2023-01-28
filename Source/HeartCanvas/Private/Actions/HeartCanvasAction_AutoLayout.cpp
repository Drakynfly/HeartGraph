// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasAction_AutoLayout.h"
#include "UMG/HeartGraphCanvas.h"
#include "ModelView/HeartLayoutHelper.h"

void UHeartCanvasAction_AutoLayout::ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation,
	UObject* ContextObject)
{
	if (UClass* Class = Cast<UClass>(ContextObject))
	{
		TSubclassOf<UHeartLayoutHelper> LayoutClass = Class;
		if (IsValid(LayoutClass))
		{
			GetDefault<UHeartLayoutHelper>(LayoutClass)->Layout(CanvasGraph);
		}
	}
}
