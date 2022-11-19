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

	FVectorBounds() {}

	FVectorBounds(const FVector& Min, const FVector& Max)
	  : Min(Min),
		Max(Max) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector2DBounds")
	FVector Min = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector2DBounds")
	FVector Max = FVector::ZeroVector;
};
