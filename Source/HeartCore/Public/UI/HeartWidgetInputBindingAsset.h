// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "Engine/DataAsset.h"
#include "HeartWidgetInputBindingAsset.generated.h"

class UHeartWidgetInputHandlerAsset;

USTRUCT(BlueprintType)
struct FHeartWidgetInputBinding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (NoResetToDefault, DisplayThumbnail = false))
	TObjectPtr<UHeartWidgetInputHandlerAsset> InputHandler;

	UPROPERTY(EditAnywhere, meta = (BaseStruct = "/Script/HeartCore.HeartWidgetInputTrigger", ExcludeBaseStruct))
	TArray<FInstancedStruct> Triggers;
};

class UHeartWidgetInputLinker;

/**
 *
 */
UCLASS(const, BlueprintType, CollapseCategories)
class HEARTCORE_API UHeartWidgetInputBindingAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "WidgetInputBindingAsset")
	TArray<FHeartWidgetInputBinding> BindingData;
};
