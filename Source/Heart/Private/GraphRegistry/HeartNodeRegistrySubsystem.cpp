// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphRegistry/HeartNodeRegistrySubsystem.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "GraphRegistry/GraphNodeRegistrar.h"

#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNodeBlueprint.h"
#include "View/HeartVisualizerInterfaces.h"

#include "AssetRegistry/AssetRegistryModule.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

DEFINE_LOG_CATEGORY(LogHeartNodeRegistry)

bool UHeartNodeRegistrySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if WITH_EDITOR
	return Super::ShouldCreateSubsystem(Outer);
#else
	return GetDefault<UHeartGraphSettings>()->CreateRuntimeNodeRegistry;
#endif
}

void UHeartNodeRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FetchNativeClasses();

#if !WITH_EDITOR
	// We can't cache blueprints in the editor during Initialize because they might not be loaded yet.
	FetchAssetRegistryAssets();
#endif
}

#if WITH_EDITOR
void UHeartNodeRegistrySubsystem::SetupBlueprintCaching()
{
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

void UHeartNodeRegistrySubsystem::OnFilesLoaded()
{
	FetchAssetRegistryAssets();
}

void UHeartNodeRegistrySubsystem::OnAssetAdded(const FAssetData& AssetData)
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
			if (auto&& NewRegistrar = Cast<UGraphNodeRegistrar>(AssetData.GetAsset()) )
			{
				AutoAddRegistrar(NewRegistrar);
			}
		}
	}
}

void UHeartNodeRegistrySubsystem::OnAssetRemoved(const FAssetData& AssetData)
{
	if (AssetData.GetClass()->IsChildOf(UGraphNodeRegistrar::StaticClass()))
	{
		auto&& NewRegistrar = Cast<UGraphNodeRegistrar>(AssetData.GetAsset());

		if (NewRegistrar)
		{
			AutoRemoveRegistrar(NewRegistrar);
		}
	}
}

void UHeartNodeRegistrySubsystem::OnHotReload(EReloadCompleteReason ReloadCompleteReason)
{
	FetchAssetRegistryAssets();
}

void UHeartNodeRegistrySubsystem::OnBlueprintPreCompile(UBlueprint* Blueprint)
{
	if (Blueprint && Blueprint->GeneratedClass && Blueprint->GeneratedClass->IsChildOf(UHeartGraphNode::StaticClass()))
	{
		WaitingForBlueprintToCompile++;
	}
}

void UHeartNodeRegistrySubsystem::OnBlueprintCompiled()
{
	WaitingForBlueprintToCompile--;

	if (WaitingForBlueprintToCompile <= 0)
	{
		WaitingForBlueprintToCompile = 0;
		FetchAssetRegistryAssets();
	}
}

#endif

UBlueprint* UHeartNodeRegistrySubsystem::GetNodeBlueprint(const FAssetData& AssetData)
{
	UBlueprint* Blueprint = Cast<UBlueprint>(AssetData.GetAsset());
	if (Blueprint && (Blueprint->GeneratedClass))
	{
		return Blueprint;
	}

	return nullptr;
}

void UHeartNodeRegistrySubsystem::FetchNativeClasses()
{
	TArray<UClass*> HeartGraphNodeClasses;
	GetDerivedClasses(UHeartGraphNode::StaticClass(), HeartGraphNodeClasses);
	for (UClass* Class : HeartGraphNodeClasses)
	{
		if (Class->IsNative())
		{
			KnownNativeClasses.GraphNodeClasses.Emplace(Class);
		}
	}

	// @todo PLEASE FIND A BETTER WAY TO DO THIS
	TArray<UClass*> VisualizerClasses;
	GetDerivedClasses(UObject::StaticClass(), VisualizerClasses);
	for (UClass* Class : VisualizerClasses)
	{
		if (Class->IsNative())
		{
			if (Class->ImplementsInterface(UGraphNodeVisualizerInterface::StaticClass()))
			{
				KnownNativeClasses.NodeVisualizerClasses.Emplace(Class);
			}
			else if (Class->ImplementsInterface(UGraphPinVisualizerInterface::StaticClass()))
			{
				KnownNativeClasses.PinVisualizerClasses.Emplace(Class);
			}
		}
	}

	// @todo we cannot autodiscover native classes of UHeartEdGraphNode as they are in the editor module
	// To fix this we would need to have the editor module inject them somehow, but preferable we wouldn't do this at all
	/*
	TArray<UClass*> GraphNodes;
	GetDerivedClasses(UHeartEdGraphNode::StaticClass(), GraphNodes);
	for (UClass* Class : GraphNodes)
	{
		auto&& DefaultObject = Class->GetDefaultObject<UHeartEdGraphNode>();
		for (UClass* AssignedClass : DefaultObject->AssignedNodeClasses)
		{
			if (AssignedClass->IsChildOf(UHeartGraphNode::StaticClass()))
			{
				AssignedGraphNodeClasses.Emplace(AssignedClass, Class);
			}
		}
	}
	*/
}

void UHeartNodeRegistrySubsystem::FetchAssetRegistryAssets()
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);

	// We can't try to load assets while they are still loading in. Try again later.
	if (AssetRegistryModule.Get().IsLoadingAssets())
	{
		return;
	}

	FARFilter RegistrarFilter;
	RegistrarFilter.ClassPaths.Add(UGraphNodeRegistrar::StaticClass()->GetClassPathName());
	RegistrarFilter.bRecursiveClasses = true;

	TArray<FAssetData> FoundRegistrarAssets;
	AssetRegistryModule.Get().GetAssets(RegistrarFilter, FoundRegistrarAssets);
	for (const FAssetData& RegistrarAsset : FoundRegistrarAssets)
	{
		if (auto&& Registrar = Cast<UGraphNodeRegistrar>(RegistrarAsset.GetAsset()))
		{
			AutoAddRegistrar(Registrar);
		}
	}

	FARFilter NodeClassFilter;
	NodeClassFilter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
	NodeClassFilter.ClassPaths.Add(UBlueprintGeneratedClass::StaticClass()->GetClassPathName());
	NodeClassFilter.bRecursiveClasses = true;

	TArray<FAssetData> FoundAssets;
	AssetRegistryModule.Get().GetAssets(RegistrarFilter, FoundAssets);
	for (const FAssetData& AssetData : FoundAssets)
	{
		if (AssetData.GetClass()->IsChildOf(UHeartGraphNodeBlueprint::StaticClass()))
		{
			if (!KnownBlueprintHeartGraphNodes.Contains(AssetData.PackageName))
			{
				KnownBlueprintHeartGraphNodes.Add(AssetData.PackageName, AssetData);

				for (auto&& NodeRegistry : NodeRegistries)
				{
					if (auto&& Blueprint = GetNodeBlueprint(AssetData))
					{
						//NodeRegistry.Value->NotifyNodeBlueprintNodeClassAdded({Blueprint->GeneratedClass});
					}
				}
			}
		}
	}
}

UHeartGraphNodeRegistry* UHeartNodeRegistrySubsystem::GetRegistry_Internal(const FSoftClassPath ClassPath)
{
#if WITH_EDITOR
	if (!HasSetupBlueprintCaching)
	{
		HasSetupBlueprintCaching = true;
		SetupBlueprintCaching();
	}
#endif

	if (auto&& FoundRegistry = NodeRegistries.Find(ClassPath))
	{
		return *FoundRegistry;
	}

	auto&& NewRegistry = NewObject<UHeartGraphNodeRegistry>(this);
	return NodeRegistries.Add(ClassPath, NewRegistry);
}

void UHeartNodeRegistrySubsystem::AutoAddRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto&& ClassPath : Registrar->AutoRegisterWith)
	{
		if (ClassPath.IsValid())
		{
			GetRegistry_Internal(ClassPath)->AddRegistrar(Registrar);
		}
	}
}

void UHeartNodeRegistrySubsystem::AutoRemoveRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto&& ClassPath : Registrar->AutoRegisterWith)
	{
		if (ClassPath.IsValid())
		{
			GetRegistry_Internal(ClassPath)->RemoveRegistrar(Registrar);
		}
	}
}

UHeartGraphNodeRegistry* UHeartNodeRegistrySubsystem::GetRegistry(const TSubclassOf<UHeartGraph> Class)
{
	return GetRegistry_Internal(FSoftClassPath(Class));
}

void UHeartNodeRegistrySubsystem::AddRegistrar(UGraphNodeRegistrar* Registrar, TSubclassOf<UHeartGraph> To)
{
	GetRegistry(To)->AddRegistrar(Registrar);
}

void UHeartNodeRegistrySubsystem::RemoveRegistrar(UGraphNodeRegistrar* Registrar, TSubclassOf<UHeartGraph> From)
{
	GetRegistry(From)->RemoveRegistrar(Registrar);
}
