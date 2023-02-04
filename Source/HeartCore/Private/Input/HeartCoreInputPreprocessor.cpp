// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartCoreInputPreprocessor.h"
#include "Input/HeartInputEventSubsystem.h"

bool FHeartCoreInputPreprocessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	return InputSubsystem.HandleMouseButtonDownEvent(SlateApp, MouseEvent);
}

bool FHeartCoreInputPreprocessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	return InputSubsystem.HandleMouseButtonUpEvent(SlateApp, MouseEvent);
}
