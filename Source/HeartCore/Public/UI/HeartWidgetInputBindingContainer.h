// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputLinker.h"
#include "Components/Widget.h"
#include "HeartWidgetInputBindingContainer.generated.h"

class UHeartWidgetInputBindingAsset;

/**
 *
 */
USTRUCT()
struct HEARTCORE_API FHeartWidgetInputBindingContainer
{
	GENERATED_BODY()

public:
	void SetupLinker(UWidget* InOuter);
	UHeartWidgetInputLinker* GetLinker() const { return Linker; }

protected:
	// Binding assets applied by default to linker
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UHeartWidgetInputBindingAsset>> BindingAssets;

	// Class of linker to spawn
	UPROPERTY(EditAnywhere)
	TSubclassOf<UHeartWidgetInputLinker> DefaultLinkerClass = UHeartWidgetInputLinker::StaticClass();

	UPROPERTY()
	TObjectPtr<UHeartWidgetInputLinker> Linker = nullptr;

	UPROPERTY()
	TWeakObjectPtr<UWidget> Outer;
};
