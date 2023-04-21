// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#if WITH_EDITOR
#include "GraphRegistry/GraphNodeRegistrar.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#endif

#if WITH_EDITOR
void UGraphNodeRegistrar::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	// Always unregister ourself before we are edited. This will prevent the registry from holding onto stuff we are no
	// longer registering.
	GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->AutoRemoveRegistrar(this);
}

void UGraphNodeRegistrar::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->AutoAddRegistrar(this);
}
#endif