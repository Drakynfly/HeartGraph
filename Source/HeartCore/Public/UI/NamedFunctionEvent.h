// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputEvent.h"
#include "NamedFunctionEvent.generated.h"

UCLASS(BlueprintType)
class HEARTCORE_API UNamedFunctionEvent : public UHeartWidgetInputEvent
{
	GENERATED_BODY()

public:
	virtual FHeartWidgetLinkedInput CreateDelegate() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString FunctionName;
};