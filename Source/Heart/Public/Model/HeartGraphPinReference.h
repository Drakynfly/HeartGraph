// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.h"
#include "HeartGraphPinReference.generated.h"

USTRUCT(BlueprintType)
struct FHeartGraphPinReference
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, BlueprintReadOnly)
	FHeartNodeGuid NodeGuid;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	FHeartPinGuid PinGuid;
};