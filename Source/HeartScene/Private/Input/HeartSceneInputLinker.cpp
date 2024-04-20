// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartSceneInputLinker.h"
#include "Input/HeartInputActivation.h"
#include "Input/HeartEvent.h"
#include "Input/HeartInputLinkerInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSceneInputLinker)

bool UHeartSceneInputLinker::InputKey(USceneComponent* Target, const FInputKeyParams& Params)
{
	return QuickTryCallbacks(Heart::Input::FInputTrip(Params), Target, FHeartInputActivation(Params)).WasEventCaptured();
}

namespace Heart::Input
{
	UHeartSceneInputLinker* TLinkerType<USceneComponent>::FindLinker(const USceneComponent* Component)
	{
		return TryFindLinker<UHeartSceneInputLinker>(Component);
	}
}