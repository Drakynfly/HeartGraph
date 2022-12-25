// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphTypes.generated.h"

class UHeartGraphNode;
class UHeartGraphPin;

USTRUCT(BlueprintType)
struct FHeartGraphConnectionEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "ConnectionEvent")
	TArray<TObjectPtr<UHeartGraphNode>> AffectedNodes;

	UPROPERTY(BlueprintReadOnly, Category = "ConnectionEvent")
	TArray<TObjectPtr<UHeartGraphPin>> AffectedPins;
};
