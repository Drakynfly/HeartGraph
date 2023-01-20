// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "Engine/DataAsset.h"
#include "HeartWidgetInputBindingAsset.generated.h"

class UHeartWidgetInputBindingBase;

USTRUCT()
struct FHeartWidgetInputBinding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UHeartWidgetInputBindingBase> BindingObject;

	UPROPERTY(EditAnywhere, meta = (BaseStruct = "/Script/HeartCore.HeartWidgetInputTrigger", ExcludeBaseStruct))
	TArray<FInstancedStruct> Triggers;
};

/**
 *
 */
UCLASS(const, BlueprintType)
class HEARTCORE_API UHeartWidgetInputBindingAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// DEPRECATED move data to BindingData
	UPROPERTY(EditAnywhere, Instanced)
	TArray<TObjectPtr<UHeartWidgetInputBindingBase>> Bindings;

	UPROPERTY(EditAnywhere)
	TArray<FHeartWidgetInputBinding> BindingData;
};