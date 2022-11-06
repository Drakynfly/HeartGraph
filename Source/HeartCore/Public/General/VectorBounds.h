// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "VectorBounds.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct HEARTCORE_API FVectorBounds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector2DBounds")
	FVector Min;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector2DBounds")
	FVector Max;
};
