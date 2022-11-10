// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "HeartWidgetInputBindingAsset.generated.h"

class UHeartWidgetInputBindingBase;

/**
 *
 */
UCLASS(const, BlueprintType)
class HEARTCORE_API UHeartWidgetInputBindingAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Instanced)
	TArray<TObjectPtr<UHeartWidgetInputBindingBase>> Bindings;
};
