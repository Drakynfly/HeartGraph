// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputLinker.h"

#include "HeartWidgetInputBindingContainer.generated.h"

class UWidget;
class UHeartInputBindingAsset;
class UHeartWidgetInputLinker;

/**
 *
 */
USTRUCT(BlueprintType)
struct HEARTCANVAS_API FHeartWidgetInputBindingContainer
{
	GENERATED_BODY()

public:
	void SetupLinker(UWidget* InOuter);
	UHeartWidgetInputLinker* GetLinker() const { return Linker; }

protected:
	// Binding assets applied by default to linker
	UPROPERTY(EditAnywhere, Category = "InputBindingConfig")
	TArray<TObjectPtr<UHeartInputBindingAsset>> BindingAssets;

	// Class of linker to spawn
	UPROPERTY(EditAnywhere, Category = "InputBindingConfig")
	TSubclassOf<UHeartWidgetInputLinker> DefaultLinkerClass = UHeartWidgetInputLinker::StaticClass();

	UPROPERTY(BlueprintReadOnly, meta = (NoResetToDefault), Category = "InputBindingContainer")
	TObjectPtr<UHeartWidgetInputLinker> Linker = nullptr;
};