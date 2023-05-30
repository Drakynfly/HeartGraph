// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "HeartGraphSettings.generated.h"

/**
 *
 */
UCLASS(config = Plugins, defaultconfig, DisplayName = "Heart Graph")
class HEART_API UHeartGraphSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	//~ Begin UDeveloperSettings
	virtual FName GetCategoryName() const override { return FName("Plugins"); }
	//~ End UDeveloperSettings

	// Enables creation of the Heart Registry Runtime Subsystem outside of the editor. Imposes a small performance hit
	// during startup, and a small memory footprint, but is required for using the Node Registries during gameplay.
	UPROPERTY(config, EditAnywhere, Category = "Runtime")
	bool CreateRuntimeRegistrySubsystem = false;

	// Visualizers to use when none other can be found via the Heart Registries
	UPROPERTY(config, EditAnywhere, Category = "Fallback", meta = (AllowedClasses = "/Script/Heart.GraphNodeRegistrar"))
	FSoftObjectPath FallbackVisualizerRegistrar;
};
