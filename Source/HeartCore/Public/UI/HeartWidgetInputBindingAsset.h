// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputBindingBase.h"
#include "InstancedStruct.h"
#include "Engine/DataAsset.h"
#include "HeartWidgetInputBindingAsset.generated.h"

class UHeartWidgetInputBindingBase;

USTRUCT()
struct FHeartWidgetInputBinding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, meta = (NoResetToDefault))
	TObjectPtr<UHeartWidgetInputBindingBase> BindingObject;

	UPROPERTY(EditAnywhere, meta = (BaseStruct = "/Script/HeartCore.HeartWidgetInputTrigger", ExcludeBaseStruct))
	TArray<FInstancedStruct> Triggers;
};

/**
 *
 */
UCLASS(const, BlueprintType, CollapseCategories)
class HEARTCORE_API UHeartWidgetInputBindingAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FHeartWidgetInputBinding> BindingData;
};
