// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst

#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "GraphRegistry/GraphNodeRegistrar.h"

#include "ModelView/HeartGraphSchema.h"

#include "HeartGraphSettings.h"

#include "AssetRegistry/AssetRegistryModule.h"

DEFINE_LOG_CATEGORY(LogHeartNodeRegistry)

bool UHeartRegistryRuntimeSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if WITH_EDITOR
	return Super::ShouldCreateSubsystem(Outer);
#else
	return Super::ShouldCreateSubsystem(Outer) && GetDefault<UHeartGraphSettings>()->CreateRuntimeRegistrySubsystem;
#endif
}

void UHeartRegistryRuntimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogHeartNodeRegistry, Log, TEXT("HeartNodeRegistrySubsystem Initialized"))

	// We can't cache blueprints in the editor during Initialize because they might not be compiled yet.
	// Instead, the HeartRegistryEditorSubsystem will load for us.
#if !WITH_EDITOR
	FetchAssetRegistryAssets();
#endif

	if (auto&& Settings = GetDefault<UHeartGraphSettings>())
	{
		FallbackRegistrar = Cast<UGraphNodeRegistrar>(Settings->FallbackVisualizerRegistrar.TryLoad());
	}
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

	IsFetchingRegistryAssets = false;
}

UHeartGraphNodeRegistry* UHeartRegistryRuntimeSubsystem::GetRegistry_Internal(const TSubclassOf<UHeartGraph> Class)
{
	check(Class);

	const FSoftClassPath Path(Class);

	if (auto&& FoundRegistry = Registries.Find(Path))
	{
		return *FoundRegistry;
	}

	const UClass* RegistryClass = UHeartGraphSchema::Get(Class)->GetRegistryClass();

	if (!IsValid(RegistryClass))
	{
		UE_LOG(LogHeartNodeRegistry, Warning, TEXT("GetRegistryClass returned invalid class for Graph '%s'!"), *Class->GetName())
		RegistryClass = UHeartGraphNodeRegistry::StaticClass();
	}

	auto&& NewRegistry = NewObject<UHeartGraphNodeRegistry>(this, RegistryClass);
	Registries.Add(Path, NewRegistry);
	BroadcastPostRegistryAdded(NewRegistry);

	NewRegistry->OnRegistryChangedNative.AddUObject(this, &ThisClass::OnRegistryChanged);

	return NewRegistry;
}

void UHeartRegistryRuntimeSubsystem::OnRegistryChanged(UHeartGraphNodeRegistry* Registry)
{
	BroadcastOnAnyRegistryChanged(Registry);
}

void UHeartRegistryRuntimeSubsystem::AutoAddRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto&& ClassPath : Registrar->AutoRegisterWith)
	{
		if (const TSubclassOf<UHeartGraph> Class = ClassPath.TryLoadClass<UHeartGraph>())
		{
			GetRegistry_Internal(Class)->AddRegistrar(Registrar);
		}
	}
}

void UHeartRegistryRuntimeSubsystem::AutoRemoveRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto&& ClassPath : Registrar->AutoRegisterWith)
	{
		if (const TSubclassOf<UHeartGraph> Class = ClassPath.TryLoadClass<UHeartGraph>())
		{
			GetRegistry_Internal(Class)->RemoveRegistrar(Registrar);
		}
	}
}

void UHeartRegistryRuntimeSubsystem::BroadcastPostRegistryAdded(UHeartGraphNodeRegistry* Registry)
{
#if WITH_EDITOR
	FEditorScriptExecutionGuard EditorScriptExecutionGuard;
#endif
	PostRegistryAddedNative.Broadcast(Registry);
	PostRegistryAdded.Broadcast(Registry);
}

void UHeartRegistryRuntimeSubsystem::BroadcastPreRegistryRemoved(UHeartGraphNodeRegistry* Registry)
{
#if WITH_EDITOR
	FEditorScriptExecutionGuard EditorScriptExecutionGuard;
#endif
	PreRegistryRemovedNative.Broadcast(Registry);
	PreRegistryRemoved.Broadcast(Registry);
}

void UHeartRegistryRuntimeSubsystem::BroadcastOnAnyRegistryChanged(UHeartGraphNodeRegistry* Registry)
{
#if WITH_EDITOR
	FEditorScriptExecutionGuard EditorScriptExecutionGuard;
#endif
	OnAnyRegistryChangedNative.Broadcast(Registry);
	OnAnyRegistryChanged.Broadcast(Registry);
}

UHeartGraphNodeRegistry* UHeartRegistryRuntimeSubsystem::GetRegistry(const TSubclassOf<UHeartGraph> Class)
{
	return GetRegistry_Internal(Class);
}

void UHeartRegistryRuntimeSubsystem::AddRegistrar(UGraphNodeRegistrar* Registrar, const TSubclassOf<UHeartGraph> To)
{
	GetRegistry(To)->AddRegistrar(Registrar);
}

void UHeartRegistryRuntimeSubsystem::RemoveRegistrar(UGraphNodeRegistrar* Registrar, const TSubclassOf<UHeartGraph> From)
{
	GetRegistry(From)->RemoveRegistrar(Registrar);
}
