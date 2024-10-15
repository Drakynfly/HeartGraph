// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "StructUtils/InstancedStruct.h"
#include "Engine/DataAsset.h"
#include "HeartInputBindingAsset.generated.h"

class UHeartInputHandlerAssetBase;

USTRUCT(BlueprintType)
struct FHeartBoundInput
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (NoResetToDefault, DisplayThumbnail = false))
	TObjectPtr<UHeartInputHandlerAssetBase> InputHandler;

	UPROPERTY(EditAnywhere, meta = (BaseStruct = "/Script/HeartCore.HeartInputTrigger", ExcludeBaseStruct))
	TArray<FInstancedStruct> Triggers;
};

/**
 *
 */
UCLASS(const, BlueprintType, CollapseCategories)
class HEARTCORE_API UHeartInputBindingAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "InputBindingAsset")
	TArray<FHeartBoundInput> BindingData;
};