// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputBinding_TriggerBase.h"
#include "HeartNamedFunctionEvent.generated.h"

UCLASS(BlueprintType, DisplayName = "Event - Named Function")
class HEARTCORE_API UHeartNamedFunctionEvent : public UHeartWidgetInputBinding_TriggerBase
{
	GENERATED_BODY()

protected:
	virtual FReply TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString FunctionName;
};