// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphRegistry/GraphNodeRegistrar.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"

#if WITH_EDITOR

/**
 * We always unregister ourself before we are edited. This will prevent the registry from holding onto stuff we are no
 * longer registering. We then have to re-auto-register ourself after the edit.
 */

void UGraphNodeRegistrar::PreEditChange(FProperty* PropertyAboutToChange)
{
	GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->AutoRemoveRegistrar(this);

	Super::PreEditChange(PropertyAboutToChange);
}

void UGraphNodeRegistrar::PreEditChange(FEditPropertyChain& PropertyAboutToChange)
{
	GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->AutoRemoveRegistrar(this);

	Super::PreEditChange(PropertyAboutToChange);
}

void UGraphNodeRegistrar::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->AutoAddRegistrar(this);
}

void UGraphNodeRegistrar::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->AutoAddRegistrar(this);
}
#endif
