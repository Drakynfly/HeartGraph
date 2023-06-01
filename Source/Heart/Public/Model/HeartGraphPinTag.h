// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"

#include "General/GameplayTagChildImplementerCopy.h"

#include "HeartGraphPinTag.generated.h"

/**
 * Tag to use to identify types of Heart Graph pins.
 */
USTRUCT(BlueprintType, meta = (Categories = "Heart.Pin"))
struct HEART_API FHeartGraphPinTag : public FGameplayTag
{
	GENERATED_BODY()

	FORCEINLINE friend uint32 GetTypeHash(const FHeartGraphPinTag& Tag)
	{
		return GetTypeHash(Tag.TagName);
	}

	END_HEART_TAG_DECL(FHeartGraphPinTag, TEXT("Heart.Pin"))
};

