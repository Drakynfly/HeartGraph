// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartGraphSettings.h"

#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphSettings)

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