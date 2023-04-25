// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"

#include "HeartGraphSettings.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "GraphRegistry/GraphNodeRegistrar.h"

#include "Model/HeartGraphNode.h"
#include "View/HeartVisualizerInterfaces.h"

#include "AssetRegistry/AssetRegistryModule.h"

DEFINE_LOG_CATEGORY(LogHeartNodeRegistry)

bool UHeartRegistryRuntimeSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if WITH_EDITOR
	return Super::ShouldCreateSubsystem(Outer);
#else
	return Super::ShouldCreateSubsystem(Outer) && GetDefault<UHeartGraphSettings>()->CreateRuntimeNodeRegistry;
#endif
}

void UHeartRegistryRuntimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogHeartNodeRegistry, Log, TEXT("HeartNodeRegistrySubsystem Initialized"))

	FetchNativeClasses();

#if !WITH_EDITOR
	// We can't cache blueprints in the editor during Initialize because they might not be compiled yet.
	// Instead, the HeartRegistryEditorSubsystem will load for us.
	FetchAssetRegistryAssets();
#endif

	if (auto&& Settings = GetDefault<UHeartGraphSettings>())
	{
		FallbackRegistrar = Cast<UGraphNodeRegistrar>(Settings->FallbackVisualizerRegistrar.TryLoad());
	}
}

void UHeartRegistryRuntimeSubsystem::FetchNativeClasses()
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

void UHeartRegistryRuntimeSubsystem::FetchAssetRegistryAssets()
{
	// @todo this is a hack to prevent this function from being recursively triggered. I'd like a cleaner solution, but this'll do...
	static bool IsFetchingRegistryAssets = false;
	if (IsFetchingRegistryAssets) return;

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);

	IsFetchingRegistryAssets = true;

	FARFilter RegistrarFilter;
	RegistrarFilter.ClassPaths.Add(UGraphNodeRegistrar::StaticClass()->GetClassPathName());
	RegistrarFilter.bRecursiveClasses = true;

	TArray<FAssetData> FoundRegistrarAssets;
	AssetRegistryModule.Get().GetAssets(RegistrarFilter, FoundRegistrarAssets);
	for (const FAssetData& RegistrarAsset : FoundRegistrarAssets)
	{
		UE_LOG(LogHeartNodeRegistry, Log, TEXT("FetchAssetRegistryAssets adding registrar '%s'"), *RegistrarAsset.GetFullName())

		if (auto&& Registrar = Cast<UGraphNodeRegistrar>(RegistrarAsset.GetAsset()))
		{
			AutoAddRegistrar(Registrar);
		}
	}

	for (auto&& RegistryTuple : Registries)
	{
		FindRecursiveClassesForRegistry(RegistryTuple.Value);
	}

	IsFetchingRegistryAssets = false;
}

void UHeartRegistryRuntimeSubsystem::FindRecursiveClassesForRegistry(UHeartGraphNodeRegistry* Registry)
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);

	FHeartRegistrationClasses Classes = Registry->GetClassesRegisteredRecursively();

	FARFilter GraphNodeClassFilter;

	for (auto&& Class : Classes.GraphNodeClasses)
	{
		if (!IsValid(Class)) continue;

		if (auto&& Blueprint = Cast<UBlueprint>(Class->ClassGeneratedBy))
		{
			GraphNodeClassFilter.ClassPaths.Add(Blueprint->StaticClass()->GetClassPathName());
		}
	}

	GraphNodeClassFilter.bRecursiveClasses = true;

	bool FoundAnyAssets = false;
	FHeartRegistrationClasses FoundClasses;

	TArray<FAssetData> FoundAssets;
	AssetRegistryModule.Get().GetAssets(GraphNodeClassFilter, FoundAssets);
	if (!FoundAssets.IsEmpty())
	{
		FoundAnyAssets = true;
	}

	for (const FAssetData& AssetData : FoundAssets)
	{
		FoundClasses.GraphNodeClasses.Add(AssetData.GetClass());
	}

	if (FoundAnyAssets)
	{
		Registry->SetRecursivelyDiscoveredClasses(FoundClasses);
	}
}

UHeartGraphNodeRegistry* UHeartRegistryRuntimeSubsystem::GetRegistry_Internal(const FSoftClassPath& ClassPath)
{
	if (auto&& FoundRegistry = Registries.Find(ClassPath))
	{
		return *FoundRegistry;
	}

	auto&& NewRegistry = NewObject<UHeartGraphNodeRegistry>(this);
	return Registries.Add(ClassPath, NewRegistry);
}

void UHeartRegistryRuntimeSubsystem::AutoAddRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto&& ClassPath : Registrar->AutoRegisterWith)
	{
		if (ClassPath.IsValid())
		{
			GetRegistry_Internal(ClassPath)->AddRegistrar(Registrar);
		}
	}
}

void UHeartRegistryRuntimeSubsystem::AutoRemoveRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto&& ClassPath : Registrar->AutoRegisterWith)
	{
		if (ClassPath.IsValid())
		{
			GetRegistry_Internal(ClassPath)->RemoveRegistrar(Registrar);
		}
	}
}

UHeartGraphNodeRegistry* UHeartRegistryRuntimeSubsystem::GetRegistry(const TSubclassOf<UHeartGraph> Class)
{
	return GetRegistry_Internal(FSoftClassPath(Class));
}

void UHeartRegistryRuntimeSubsystem::AddRegistrar(UGraphNodeRegistrar* Registrar, const TSubclassOf<UHeartGraph> To)
{
	GetRegistry(To)->AddRegistrar(Registrar);
}

void UHeartRegistryRuntimeSubsystem::RemoveRegistrar(UGraphNodeRegistrar* Registrar, const TSubclassOf<UHeartGraph> From)
{
	GetRegistry(From)->RemoveRegistrar(Registrar);
}
