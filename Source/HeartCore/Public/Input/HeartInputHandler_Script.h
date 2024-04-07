// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartInputHandler_Immediate.h"
#include "HeartInputHandler_Script.generated.h"

/**
 *
 */
UCLASS(Blueprintable, Abstract)
class HEARTCORE_API UHeartInputHandler_Script : public UHeartInputHandler_Immediate
{
	GENERATED_BODY()

public:
	virtual FHeartEvent OnTriggered(UObject* Target, const FHeartInputActivation& Trip) const override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|InputHandlerScript")
	FHeartEvent HandleEvent(UObject* Widget, const FHeartInputActivation& Trip) const;
};