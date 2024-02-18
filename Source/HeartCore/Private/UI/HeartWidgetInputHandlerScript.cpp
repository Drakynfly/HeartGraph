// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputHandlerScript.h"

#include "Components/SlateWrapperTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartWidgetInputHandlerScript)

FReply UHeartWidgetInputHandlerScript::TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const
{
	return HandleEvent(Widget, Trip).NativeReply;
}