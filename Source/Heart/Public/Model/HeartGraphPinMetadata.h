// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGraphPinMetadata.generated.h"

/**
 * Base class for per-pin instanced metadata.
 */
UCLASS(Blueprintable, BlueprintType, Abstract, DefaultToInstanced)
class HEART_API UHeartGraphPinMetadata : public UObject
{
	GENERATED_BODY()
};