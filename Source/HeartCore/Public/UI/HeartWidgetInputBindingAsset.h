// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputBindingBase.h"
#include "HeartWidgetInputBindingContainer.h"
#include "InstancedStruct.h"
#include "Engine/DataAsset.h"
#include "HeartWidgetInputBindingAsset.generated.h"

USTRUCT()
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
	UPROPERTY(EditAnywhere)
	TArray<FHeartWidgetInputBinding> BindingData;

	void BindLinker(const TObjectPtr<UHeartWidgetInputLinker>& Linker);
	void UnbindLinker(const TObjectPtr<UHeartWidgetInputLinker>& Linker);
};
