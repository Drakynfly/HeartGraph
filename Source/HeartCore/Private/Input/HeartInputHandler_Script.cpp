﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputHandler_Script.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputHandler_Script)

FHeartEvent UHeartInputHandler_Script::OnTriggered(UObject* Target, const FHeartInputActivation& Trip) const
{
	return HandleEvent(Target, Trip);
}