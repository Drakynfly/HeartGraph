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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VectorBounds")
	FVector Min = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VectorBounds")
	FVector Max = FVector::ZeroVector;
};

/**
 *
 */
USTRUCT(BlueprintType)
struct HEARTCORE_API FVector3fBounds
{
	GENERATED_BODY()

	FVector3fBounds() {}

	FVector3fBounds(const FVector3f& Min, const FVector3f& Max)
	  : Min(Min),
		Max(Max) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector3fBounds")
	FVector3f Min = FVector3f::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector3fBounds")
	FVector3f Max = FVector3f::ZeroVector;
};