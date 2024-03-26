// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartInputHandlerBase.h"
#include "HeartInputHandler_Script.generated.h"

struct FEventReply;

/**
 *
 */
UCLASS(Blueprintable, Abstract)
class HEARTCORE_API UHeartInputHandler_Script : public UHeartInputHandlerBase
{
	GENERATED_BODY()

public:
	virtual FReply OnTriggered(UObject* Target, const FHeartInputActivation& Trip) const override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|InputHandlerScript")
	FEventReply HandleEvent(UObject* Widget, const FHeartInputActivation& Trip) const;
};