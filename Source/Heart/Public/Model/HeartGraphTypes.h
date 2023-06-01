// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.h"
#include "HeartGraphTypes.generated.h"

class UHeartGraphNode;

USTRUCT(BlueprintType)
struct FHeartGraphConnectionEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "ConnectionEvent")
	TArray<TObjectPtr<UHeartGraphNode>> AffectedNodes;

	UPROPERTY(BlueprintReadOnly, Category = "ConnectionEvent")
	TArray<FHeartPinGuid> AffectedPins;
};

USTRUCT(BlueprintType)
struct FHeartGraphNodeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "NodeMessage")
	FText Message;

	UPROPERTY(BlueprintReadOnly, Category = "NodeMessage")
	FLinearColor Color = FLinearColor::White;
};


