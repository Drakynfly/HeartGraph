// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartRegistryEditorSubsystem.h"

#include "HeartEditorModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "General/HeartGeneralUtils.h"
#include "GraphRegistry/GraphNodeRegistrar.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "Model/HeartGraphNode.h"
#include "Nodes/HeartEdGraphNode.h"

void UHeartRegistryEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	BindToRuntimeSubsystem();
	SubscribeToAssetChanges();
	FetchAssetRegistryAssets();
}

void UHeartRegistryEditorSubsystem::Deinitialize()
{
	// Unbind from the runtime subsystem
	if (auto&& RuntimeSS = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		RuntimeSS->GetPostRegistryAddedNative().RemoveAll(this);
		RuntimeSS->GetPreRegistryRemovedNative().RemoveAll(this);
		RuntimeSS->GetOnAnyRegistryChangedNative().RemoveAll(this);
	}

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

TArray<UClass*> UHeartRegistryEditorSubsystem::GetFactoryCommonClasses()
{
	return UHeartGeneralUtils::GetChildClasses(UHeartGraph::StaticClass(), false).FilterByPredicate(
		[](const UClass* Class)
		{
			if (Class->IsChildOf<UHeartGraph>())
			{
				return Class->GetDefaultObject<UHeartGraph>()->GetDisplayClassAsCommonInFactory();
			}
			return false;
		});
}

TSharedPtr<SGraphNode> UHeartRegistryEditorSubsystem::MakeVisualWidget(const FName Style, UHeartEdGraphNode* Node) const
{
	const FHeartEditorModule& HeartEditorModule = FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");
	return HeartEditorModule.MakeVisualWidget(Style, Node);
}

UClass* UHeartRegistryEditorSubsystem::GetAssignedEdGraphNodeClass(
	const TSubclassOf<UHeartGraphNode> HeartGraphNodeClass) const
{
	const FHeartEditorModule& HeartEditorModule = FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");
	return HeartEditorModule.GetEdGraphClass(HeartGraphNodeClass);
}

void UHeartRegistryEditorSubsystem::BindToRuntimeSubsystem()
{
	if (auto&& RuntimeSS = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		RuntimeSS->GetPostRegistryAddedNative().AddUObject(this, &ThisClass::PostRegistryRemoved);
		RuntimeSS->GetPreRegistryRemovedNative().AddUObject(this, &ThisClass::PreRegistryAdded);
		RuntimeSS->GetOnAnyRegistryChangedNative().AddUObject(this, &ThisClass::OnAnyRegistryChanged);
	}
}

void UHeartRegistryEditorSubsystem::SubscribeToAssetChanges()
{
	const FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);

	AssetRegistry.Get().WaitForCompletion();
	AssetRegistry.Get().OnFilesLoaded().AddUObject(this, &ThisClass::OnFilesLoaded);
	AssetRegistry.Get().OnAssetAdded().AddUObject(this, &ThisClass::OnAssetAdded);
	AssetRegistry.Get().OnAssetRemoved().AddUObject(this, &ThisClass::OnAssetRemoved);

	FCoreUObjectDelegates::ReloadCompleteDelegate.AddUObject(this, &ThisClass::OnHotReload);

	if (GEditor)
	{
		GEditor->OnBlueprintPreCompile().AddUObject(this, &ThisClass::OnBlueprintPreCompile);
		GEditor->OnBlueprintCompiled().AddUObject(this, &ThisClass::OnBlueprintCompiled);
	}
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

	if (const UClass* AssetClass = AssetData.GetClass())
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

void UHeartRegistryEditorSubsystem::PreRegistryAdded(UHeartGraphNodeRegistry* HeartGraphNodeRegistry)
{
	OnRefreshPalettes.Broadcast();
}

void UHeartRegistryEditorSubsystem::PostRegistryRemoved(UHeartGraphNodeRegistry* HeartGraphNodeRegistry)
{
	OnRefreshPalettes.Broadcast();
}

void UHeartRegistryEditorSubsystem::OnAnyRegistryChanged(UHeartGraphNodeRegistry* HeartGraphNodeRegistry)
{
	OnRefreshPalettes.Broadcast();
}

void UHeartRegistryEditorSubsystem::FetchAssetRegistryAssets()
{
	GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>()->FetchAssetRegistryAssets();

	OnRefreshPalettes.Broadcast();
}