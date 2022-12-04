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
	//~ Begin UDeveloperSettings
	virtual FName GetCategoryName() const override { return FName(TEXT("Plugins")); }
	//~ End UDeveloperSettings

	// Enables use of the Node Registry Subsystem at runtime. Imposes a small performance hit during startup, and a
	// small memory footprint.
	UPROPERTY(config, EditAnywhere, Category = "Runtime")
	bool CreateRuntimeNodeRegistry = false;

	UPROPERTY(config, EditAnywhere, Category = "Fallback", meta = (AllowedClasses = "/Script/Heart.GraphNodeRegistrar"))
	FSoftObjectPath FallbackVisualizerRegistrar;
};
