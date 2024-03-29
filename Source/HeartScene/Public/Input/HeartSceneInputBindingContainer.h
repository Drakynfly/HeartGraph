// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartSceneInputLinker.h"

#include "HeartSceneInputBindingContainer.generated.h"

class UHeartInputBindingAsset;

/**
 *
 */
USTRUCT(BlueprintType)
struct HEARTSCENE_API FHeartSceneInputBindingContainer
{
	GENERATED_BODY()

	void SetupLinker(UObject* InOuter);
	UHeartSceneInputLinker* GetLinker() const { return Linker; }

protected:
	// Binding assets applied by default to linker
	UPROPERTY(EditAnywhere, Category = "InputBindingConfig")
	TArray<TObjectPtr<UHeartInputBindingAsset>> BindingAssets;

	// Class of linker to spawn
	UPROPERTY(EditAnywhere, Category = "InputBindingConfig")
	TSubclassOf<UHeartSceneInputLinker> DefaultLinkerClass = UHeartSceneInputLinker::StaticClass();

	UPROPERTY(BlueprintReadOnly, meta = (NoResetToDefault), Category = "InputBindingContainer")
	TObjectPtr<UHeartSceneInputLinker> Linker = nullptr;
};