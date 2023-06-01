// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputBinding_TriggerBase.h"
#include "HeartWidgetInputHandlerScript.generated.h"

struct FEventReply;

/**
 *
 */
UCLASS(Blueprintable, Abstract)
class HEARTCORE_API UHeartWidgetInputHandlerScript : public UHeartWidgetInputBinding_TriggerBase
{
	GENERATED_BODY()

public:
	virtual FReply TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|InputHandlerScript")
	FEventReply HandleEvent(UWidget* Widget, const FHeartInputActivation& Trip) const;
};
