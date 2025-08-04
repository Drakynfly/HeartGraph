// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphRegistry/GraphNodeRegistrar.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "Model/HeartGraphNode.h"

#include "Engine/AssetManager.h"
#include "Engine/Engine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GraphNodeRegistrar)

#if WITH_EDITOR

void UGraphNodeRegistrar::PostLoad()
{
	Super::PostLoad();

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	if (!Registration.GraphNodeLists.IsEmpty())
	{
		for (auto&& Element : Registration.GraphNodeLists)
		{
			auto& List = ClassLists.NodeLists.Add(Element.Key.Get());
			for (auto&& Class : Element.Value.Classes)
			{
				FHeartRegistryClass& RegistryClass = List.Classes.AddDefaulted_GetRef();
				RegistryClass.Class = Class.Class.Get();
				RegistryClass.Recursive = Class.Recursive;
			}
			for (auto&& Object : Element.Value.Objects)
			{
				List.Objects.Add(Object);
			}
			for (auto&& Visualizer : Element.Value.Visualizers)
			{
				List.Visualizers.Add(Visualizer.Get());
			}
		}
		Registration.GraphNodeLists.Reset();
		(void)MarkPackageDirty();
	}
	if (!Registration.GraphPinLists.IsEmpty())
	{
		for (auto&& Element : Registration.GraphPinLists)
		{
			auto& List = ClassLists.PinLists.Add(Element.Key);
			for (auto&& PinVisualizer : Element.Value.PinVisualizers)
			{
				List.PinVisualizers.Add(PinVisualizer.Get());
			}
			for (auto&& ConnectionVisualizer : Element.Value.ConnectionVisualizers)
			{
				List.ConnectionVisualizers.Add(ConnectionVisualizer.Get());
			}
		}
		Registration.GraphPinLists.Reset();
		(void)MarkPackageDirty();
	}
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

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
	return IsAsset() || GetClass() != UGraphNodeRegistrar::StaticClass();
}

void UGraphNodeRegistrar::OnRegistered(UHeartGraphNodeRegistry* Registry) const
{
	TArray<FSoftObjectPath> ObjectsToLoad;
	UHeartGraphNodeRegistry::GatherReferences(ClassLists, ObjectsToLoad);

	if (ObjectsToLoad.IsEmpty())
	{
		RegisterAssets(Registry);
	}
	else
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad(MoveTemp(ObjectsToLoad),
			FStreamableDelegate::CreateUObject(this, &ThisClass::RegisterAssets, Registry));
	}
}

void UGraphNodeRegistrar::OnDeregistered(UHeartGraphNodeRegistry* Registry) const
{
	Registry->RemoveRegistrationList(ClassLists);

#if WITH_EDITOR
	FEditorScriptExecutionGuard ScriptExecutionGuard;
#endif
	BP_Deregister(Registry);
}

void UGraphNodeRegistrar::RegisterAssets(UHeartGraphNodeRegistry* Registry) const
{
	if (!IsValid(Registry))
	{
		return;
	}

	Registry->AddRegistrationList(ClassLists);

#if WITH_EDITOR
	FEditorScriptExecutionGuard ScriptExecutionGuard;
#endif
	BP_Register(Registry);
}