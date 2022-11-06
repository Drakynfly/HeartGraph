// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Vector2DBounds.generated.h"

USTRUCT(BlueprintType)
struct FVector2DBounds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector2DBounds")
	FVector2D Min;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector2DBounds")
	FVector2D Max;
};
