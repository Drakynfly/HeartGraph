// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartGraphSettings.h"

#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"

#if WITH_EDITOR
void UHeartGraphSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, FallbackVisualizerRegistrar))
	{
		GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->LoadFallbackRegistrar();
	}
}
#endif