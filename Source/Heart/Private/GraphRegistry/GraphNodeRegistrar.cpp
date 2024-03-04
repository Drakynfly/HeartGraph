// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphRegistry/GraphNodeRegistrar.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GraphNodeRegistrar)

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

bool UGraphNodeRegistrar::ShouldRegister() const
{
	// Children of the base class should attempt to register their CDO.
	return GetClass() != UGraphNodeRegistrar::StaticClass();
}

void UGraphNodeRegistrar::OnRegistered(UHeartGraphNodeRegistry* Registry) const
{
	Registry->AddRegistrationList(Registration);

#if WITH_EDITOR
	FEditorScriptExecutionGuard ScriptExecutionGuard;
#endif
	BP_Register(Registry);
}

void UGraphNodeRegistrar::OnDeregistered(UHeartGraphNodeRegistry* Registry) const
{
	Registry->RemoveRegistrationList(Registration);

#if WITH_EDITOR
	FEditorScriptExecutionGuard ScriptExecutionGuard;
#endif
	BP_Deregister(Registry);
}