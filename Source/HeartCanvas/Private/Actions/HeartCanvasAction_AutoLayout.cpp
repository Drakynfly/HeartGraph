// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasAction_AutoLayout.h"
#include "UMG/HeartGraphCanvas.h"
#include "ModelView/HeartLayoutHelper.h"

FEventReply UHeartCanvasAction_AutoLayout::ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation,
	UObject* ContextObject)
{
	const UHeartLayoutHelper* LayoutHelper = nullptr;

	if (const UHeartLayoutHelper* AsObject = Cast<UHeartLayoutHelper>(ContextObject))
	{
		LayoutHelper = AsObject;
	}

	if (UClass* AsClass = Cast<UClass>(ContextObject))
	{
		const TSubclassOf<UHeartLayoutHelper> LayoutClass = AsClass;
		if (IsValid(LayoutClass) && !LayoutClass->HasAnyClassFlags(CLASS_Abstract))
		{
			LayoutHelper = GetDefault<UHeartLayoutHelper>(LayoutClass);
		}
	}

	if (IsValid(LayoutHelper))
	{
		LayoutHelper->Layout(CanvasGraph);
		return true;
	}

	UE_LOG(LogHeartGraphCanvas, Error, TEXT("UHeartCanvasAction_AutoLayout expects a context action that is a instance or class pointer of a UHeartLayoutHelper type."))
	return false;
}
