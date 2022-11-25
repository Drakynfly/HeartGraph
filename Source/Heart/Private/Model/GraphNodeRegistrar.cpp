// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/GraphNodeRegistrar.h"

#include "Model/HeartNodeRegistrySubsystem.h"

#if WITH_EDITOR
void UGraphNodeRegistrar::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	// Always unregister ourself before we are edited. This will prevent the registry from holding onto stuff we are no
	// longer registering.
	GEngine->GetEngineSubsystem<UHeartNodeRegistrySubsystem>()->RemoveRegistrar(this);
}

void UGraphNodeRegistrar::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// If AutoRegister is enabled, re-add ourself to the registry again, now that the edit is done.
	if (AutoRegister)
	{
		GEngine->GetEngineSubsystem<UHeartNodeRegistrySubsystem>()->AddRegistrar(this);
	}
}
#endif