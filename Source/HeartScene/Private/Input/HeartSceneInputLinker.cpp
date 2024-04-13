// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartSceneInputLinker.h"
#include "Input/HeartInputActivation.h"
#include "Input/HeartEvent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSceneInputLinker)

bool UHeartSceneInputLinker::InputKey(const FInputKeyParams& Params, UObject* Target)
{
	return QuickTryCallbacks(FHeartInputTrip(Params), Target, FHeartInputActivation(Params)).WasEventCaptured();
}