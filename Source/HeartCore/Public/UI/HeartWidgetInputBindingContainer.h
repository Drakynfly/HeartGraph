// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Components/Widget.h"
#include "Move_To_UMG/HeartUMGInputLinker.h"
#include "HeartWidgetInputBindingContainer.generated.h"

class UHeartInputBindingAsset;

/**
 *
 */
USTRUCT(BlueprintType)
struct HEARTCORE_API FHeartWidgetInputBindingContainer
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

	UPROPERTY()
	TWeakObjectPtr<UWidget> Outer;
};