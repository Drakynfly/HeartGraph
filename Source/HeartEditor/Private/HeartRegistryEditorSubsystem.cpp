// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartRegistryEditorSubsystem.h"

#include "HeartEditorModule.h"
#include "General/HeartGeneralUtils.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "Model/HeartGraphNode.h"
#include "Nodes/HeartEdGraphNode.h"
#include "View/HeartVisualizerInterfaces.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartRegistryEditorSubsystem)

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
				if (Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Hidden | CLASS_NewerVersionExists))
				{
					return false;
				}

				return Class->GetDefaultObject<UHeartGraph>()->GetDisplayClassAsCommonInFactory();
			}
			return false;
		});
}

TSharedPtr<SGraphNode> UHeartRegistryEditorSubsystem::MakeSlateWidget(const FName Style, UHeartEdGraphNode* Node) const
{
	const FHeartEditorModule& HeartEditorModule = FModuleManager::GetModuleChecked<FHeartEditorModule>("HeartEditor");
	return HeartEditorModule.MakeSlateWidget(Style, Node);
}

TSubclassOf<UHeartEdGraphNode> UHeartRegistryEditorSubsystem::GetAssignedEdGraphNodeClass(
	const TSubclassOf<UHeartGraphNode> HeartGraphNodeClass) const
{
	const FHeartEditorModule& HeartEditorModule = FModuleManager::GetModuleChecked<FHeartEditorModule>("HeartEditor");
	return HeartEditorModule.GetEdGraphClass(HeartGraphNodeClass);
}

void UHeartRegistryEditorSubsystem::RefreshAssetRegistryAssets()
{
	auto&& RuntimeSubsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>();

	RuntimeSubsystem->RefreshAssetRegistrars(true);

	OnRefreshPalettes.Broadcast();
}

bool UHeartRegistryEditorSubsystem::BlueprintImplementsHeartVisualizerInterface(const UBlueprint* Blueprint) const
{
	static const TArray<TObjectPtr<UClass>> HeartVisualizerInterfaces{
		UGraphNodeVisualizerInterface::StaticClass(),
		UGraphPinVisualizerInterface::StaticClass(),
		UGraphConnectionVisualizerInterface::StaticClass()
	};

	for (auto&& HeartVisualizerInterface : HeartVisualizerInterfaces)
	{
		if (Blueprint->GeneratedClass->ImplementsInterface(HeartVisualizerInterface))
		{
			return true;
		}
	}

	return false;
}

void UHeartRegistryEditorSubsystem::OnHotReload(EReloadCompleteReason ReloadCompleteReason)
{
	UE_LOG(LogHeartNodeRegistry, Log, TEXT("-- Running RefreshAssetRegistryAssets: OnHotReload"))
	RefreshAssetRegistryAssets();
}

void UHeartRegistryEditorSubsystem::OnBlueprintPreCompile(UBlueprint* Blueprint)
{
	if (!(Blueprint && Blueprint->GeneratedClass))
	{
		return;
	}

	if (Blueprint->GeneratedClass->IsChildOf(UHeartGraphNode::StaticClass()) ||
		BlueprintImplementsHeartVisualizerInterface(Blueprint))
	{
		Blueprint->OnCompiled().AddUObject(this, &ThisClass::OnHeartBlueprintCompiled);
		WaitingForCompilationToFinish = true;
		WaitingForCompile.Add(Blueprint);
	}
}

void UHeartRegistryEditorSubsystem::OnBlueprintCompiled()
{
	// Still compiling something else?
	if (GCompilingBlueprint) return;

	if (WaitingForCompilationToFinish)
	{
		// After compilation is done, refresh registry assets.
		check(WaitingForCompile.IsEmpty())

		WaitingForCompilationToFinish = false;

		UE_LOG(LogHeartNodeRegistry, Log, TEXT("-- Running RefreshAssetRegistryAssets: OnBlueprintCompiled"))
		RefreshAssetRegistryAssets();
	}
}

void UHeartRegistryEditorSubsystem::OnHeartBlueprintCompiled(UBlueprint* Blueprint)
{
	Blueprint->OnCompiled().RemoveAll(this);

	WaitingForCompile.Remove(Blueprint);

	if (WaitingForCompile.IsEmpty())
	{
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