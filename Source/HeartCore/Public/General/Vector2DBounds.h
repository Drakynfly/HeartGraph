// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Vector2DBounds.generated.h"

USTRUCT(BlueprintType)
struct FVector2DBounds
{
	GENERATED_BODY()

	FVector2DBounds() = default;

	FVector2DBounds(const FVector2D& Min, const FVector2D& Max)
	  : Min(Min),
		Max(Max) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector2DBounds")
	FVector2D Min = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector2DBounds")
	FVector2D Max = FVector2D::ZeroVector;
};
