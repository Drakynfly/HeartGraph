// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphPinType.h"
#include "HeartGraphPinDesc.generated.h"

enum class EHeartPinDirection : uint8;

USTRUCT(BlueprintType)
struct FHeartGraphPinDesc
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Heart|PinDesc")
	FName PinName;

	UPROPERTY(BlueprintReadWrite, Category = "Heart|PinDesc")
	FText PinFriendlyName;

	UPROPERTY(BlueprintReadWrite, Category = "Heart|PinDesc")
	FText PinTooltip;

	UPROPERTY(BlueprintReadWrite, Category = "Heart|PinDesc")
	FHeartGraphPinType Type;

	UPROPERTY(BlueprintReadWrite, Category = "Heart|PinDesc")
	EHeartPinDirection PinDirection;
};