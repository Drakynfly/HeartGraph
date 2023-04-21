// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartRegistryEditorSubsystem.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "GraphRegistry/GraphNodeRegistrar.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "Model/HeartGraphNode.h"

void UHeartRegistryEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
	AssetRegistry.Get().OnFilesLoaded().AddUObject(this, &ThisClass::OnFilesLoaded);
	AssetRegistry.Get().OnAssetAdded().AddUObject(this, &ThisClass::OnAssetAdded);
	AssetRegistry.Get().OnAssetRemoved().AddUObject(this, &ThisClass::OnAssetRemoved);

	FCoreUObjectDelegates::ReloadCompleteDelegate.AddUObject(this, &ThisClass::OnHotReload);

	if (GEditor)
	{
		GEditor->OnBlueprintPreCompile().AddUObject(this, &ThisClass::OnBlueprintPreCompile);
		GEditor->OnBlueprintCompiled().AddUObject(this, &ThisClass::OnBlueprintCompiled);
	}

	FetchAssetRegistryAssets();
}

void UHeartRegistryEditorSubsystem::Deinitialize()
{
	if (FModuleManager::Get().IsModuleLoaded(AssetRegistryConstants::ModuleName))
	{
		const FAssetRegistryModule& AssetRegistry = FModuleManager::GetModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
		AssetRegistry.Get().OnFilesLoaded().RemoveAll(this);
		AssetRegistry.Get().OnAssetAdded().RemoveAll(this);
		AssetRegistry.Get().OnAssetRemoved().RemoveAll(this);
	}

	FCoreUObjectDelegates::ReloadCompleteDelegate.RemoveAll(this);

	if (GEditor)
	{
		GEditor->OnBlueprintPreCompile().RemoveAll(this);
		GEditor->OnBlueprintCompiled().RemoveAll(this);
	}

	Super::Deinitialize();
}

void UHeartRegistryEditorSubsystem::OnFilesLoaded()
{
	FetchAssetRegistryAssets();
}

void UHeartRegistryEditorSubsystem::OnAssetAdded(const FAssetData& AssetData)
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);

	// We can't try to load assets while they are still loading in. Try again later.
	if (AssetRegistryModule.Get().IsLoadingAssets())
	{
		return;
	}

	if (auto&& AssetClass = AssetData.GetClass())
	{
		if (AssetClass->IsChildOf(UGraphNodeRegistrar::StaticClass()))
		{
			if (auto&& NewRegistrar = Cast<UGraphNodeRegistrar>(AssetData.GetAsset()))
			{
				GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->AutoAddRegistrar(NewRegistrar);
			}
		}
	}
}

void UHeartRegistryEditorSubsystem::OnAssetRemoved(const FAssetData& AssetData)
{
	if (AssetData.GetClass()->IsChildOf(UGraphNodeRegistrar::StaticClass()))
	{
		auto&& RemovedRegistrar = Cast<UGraphNodeRegistrar>(AssetData.GetAsset());

		if (IsValid(RemovedRegistrar))
		{
			GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->AutoRemoveRegistrar(RemovedRegistrar);
		}
	}
}

void UHeartRegistryEditorSubsystem::OnHotReload(EReloadCompleteReason ReloadCompleteReason)
{
	FetchAssetRegistryAssets();
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void UHeartRegistryEditorSubsystem::OnBlueprintPreCompile(UBlueprint* Blueprint)
{
	if (Blueprint && Blueprint->GeneratedClass &&
		Blueprint->GeneratedClass->IsChildOf(UHeartGraphNode::StaticClass()))
	{
		WaitingForBlueprintToCompile++;
	}
}

void UHeartRegistryEditorSubsystem::OnBlueprintCompiled()
{
	WaitingForBlueprintToCompile--;

	if (WaitingForBlueprintToCompile <= 0)
	{
		WaitingForBlueprintToCompile = 0;
		FetchAssetRegistryAssets();
	}
}

void UHeartRegistryEditorSubsystem::FetchAssetRegistryAssets()
{
	GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->FetchAssetRegistryAssets();
}

UBlueprint* UHeartRegistryEditorSubsystem::GetNodeBlueprint(const FAssetData& AssetData) const
{
	UBlueprint* Blueprint = Cast<UBlueprint>(AssetData.GetAsset());
	if (Blueprint && (Blueprint->GeneratedClass))
	{
		return Blueprint;
	}

	return nullptr;
}
