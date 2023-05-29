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

	if (auto&& RuntimeSS = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		RuntimeSS->GetPostRegistryAddedNative().AddUObject(this, &ThisClass::PostRegistryRemoved);
		RuntimeSS->GetPreRegistryRemovedNative().AddUObject(this, &ThisClass::PreRegistryAdded);
		RuntimeSS->GetOnAnyRegistryChangedNative().AddUObject(this, &ThisClass::OnAnyRegistryChanged);
	}

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
	// Unbind from the runtime subsystem
	if (auto&& RuntimeSS = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>())
	{
		RuntimeSS->GetPostRegistryAddedNative().RemoveAll(this);
		RuntimeSS->GetPreRegistryRemovedNative().RemoveAll(this);
		RuntimeSS->GetOnAnyRegistryChangedNative().RemoveAll(this);
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
	auto&& RuntimeSubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>();

	// Flush all registries so it won't disregard re-added registrars.
	for (auto&& Registry : RuntimeSubsystem->Registries)
	{
		Registry.Value->DeregisterAll();
	}

	RuntimeSubsystem->FetchAssetRegistryAssets();

	OnRefreshPalettes.Broadcast();
}