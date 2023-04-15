// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputHandlerScript.h"

FReply UHeartWidgetInputHandlerScript::TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const
{
	return HandleEvent(Widget, Trip).NativeReply;
}
