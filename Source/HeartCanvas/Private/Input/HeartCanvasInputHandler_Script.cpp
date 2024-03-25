// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartCanvasInputHandler_Script.h"

#include "Components/SlateWrapperTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasInputHandler_Script)

FReply UHeartCanvasInputHandler_Script::OnTriggered(UWidget* Widget, const FHeartInputActivation& Trip) const
{
	return HandleEvent(Widget, Trip).NativeReply;
}