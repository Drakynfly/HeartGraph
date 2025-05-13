// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst

#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "GraphRegistry/GraphNodeRegistrar.h"

#include "Model/HeartGraph.h"
#include "ModelView/HeartGraphSchema.h"

#include "HeartGraphSettings.h"

#include "Engine/AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartRegistryRuntimeSubsystem)

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

	UE_LOG(LogHeartNodeRegistry, Log, TEXT("HeartRegistryRuntimeSubsystem Initialized"))

	FetchNativeRegistrars();

	UAssetManager::CallOrRegister_OnAssetManagerCreated(
		FSimpleMulticastDelegate::FDelegate::CreateLambda([&]()
		{
			IAssetRegistry& AssetRegistry = UAssetManager::Get().GetAssetRegistry();
			AssetRegistry.OnFilesLoaded().AddUObject(this, &ThisClass::OnFilesLoaded);
			AssetRegistry.OnAssetAdded().AddUObject(this, &ThisClass::OnAssetAdded);
			AssetRegistry.OnAssetRemoved().AddUObject(this, &ThisClass::OnAssetRemoved);

			LoadFallbackRegistrar();

			UE_LOG(LogHeartNodeRegistry, Log, TEXT("-- Running FetchAssetRegistrars: Initialize"))
			FetchAssetRegistrars();
		}));
}

void UHeartRegistryRuntimeSubsystem::Deinitialize()
{
	if (FModuleManager::Get().IsModuleLoaded(AssetRegistryConstants::ModuleName))
	{
		IAssetRegistry& AssetRegistry = UAssetManager::Get().GetAssetRegistry();
		AssetRegistry.OnFilesLoaded().RemoveAll(this);
		AssetRegistry.OnAssetAdded().RemoveAll(this);
		AssetRegistry.OnAssetRemoved().RemoveAll(this);
	}
}

void UHeartRegistryRuntimeSubsystem::OnFilesLoaded()
{
	UE_LOG(LogHeartNodeRegistry, Log, TEXT("-- Running FetchAssetRegistrars: OnFilesLoaded"))
	FetchAssetRegistrars();
}

void UHeartRegistryRuntimeSubsystem::OnAssetAdded(const FAssetData& AssetData)
{
	const IAssetRegistry& AssetRegistry = UAssetManager::Get().GetAssetRegistry();

	// We can't try to load assets while they are still loading in. Try again later.
	if (AssetRegistry.IsLoadingAssets())
	{
		return;
	}

	if (AssetData.IsInstanceOf(UGraphNodeRegistrar::StaticClass()))
	{
		KnownRegistrars.Add(AssetData);

		if (auto&& NewRegistrar = Cast<UGraphNodeRegistrar>(AssetData.GetAsset()))
		{
			UE_LOG(LogHeartNodeRegistry, Log, TEXT("HeartRegistryRuntimeSubsystem OnAssetAdded detected Registrar '%s'"), *NewRegistrar->GetName())

			AutoAddRegistrar(NewRegistrar);
		}
	}
}

void UHeartRegistryRuntimeSubsystem::OnAssetRemoved(const FAssetData& AssetData)
{
	if (AssetData.IsInstanceOf(UGraphNodeRegistrar::StaticClass()))
	{
		UE_LOG(LogHeartNodeRegistry, Log, TEXT("HeartRegistryRuntimeSubsystem OnAssetRemoved detected Registrar"))

		if (AssetData.IsAssetLoaded())
		{
			KnownRegistrars.Remove(AssetData);

			if (auto&& RemovedRegistrar = Cast<UGraphNodeRegistrar>(AssetData.GetAsset()))
			{
				if (IsValid(RemovedRegistrar))
				{
					AutoRemoveRegistrar(RemovedRegistrar);
				}
			}
		}
	}
}

void UHeartRegistryRuntimeSubsystem::LoadFallbackRegistrar()
{
	if (auto&& Settings = GetDefault<UHeartGraphSettings>())
	{
		FallbackRegistrar = Cast<UGraphNodeRegistrar>(Settings->FallbackVisualizerRegistrar.TryLoad());
	}
}

void UHeartRegistryRuntimeSubsystem::FetchNativeRegistrars()
{
	static constexpr EClassFlags BannedClassFlags = CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_Abstract;

	TArray<UClass*> Classes;
	GetDerivedClasses(UGraphNodeRegistrar::StaticClass(), Classes);

	for (const TSubclassOf<UGraphNodeRegistrar>& Class : Classes)
	{
		if (IsValid(Class))
		{
			// Skip classes with banned flags
			if (Class->HasAnyClassFlags(BannedClassFlags))
			{
				continue;
			}

			const UGraphNodeRegistrar* RegistrarDefault = GetDefault<UGraphNodeRegistrar>(Class);
			AutoAddRegistrar(RegistrarDefault);
		}
	}
}

void UHeartRegistryRuntimeSubsystem::FetchAssetRegistrars()
{
	// @todo this is a hack to prevent this function from being recursively triggered. I'd like a cleaner solution, but this'll do...
	static bool IsFetchingRegistryAssets = false;
	if (IsFetchingRegistryAssets) return;
	TGuardValue<bool> RecursionGuard(IsFetchingRegistryAssets, true);

	QUICK_SCOPE_CYCLE_COUNTER(STAT_FetchAssetRegistryAssets)

	IAssetRegistry& AssetRegistry = UAssetManager::Get().GetAssetRegistry();

	FARFilter RegistrarFilter;
	RegistrarFilter.ClassPaths.Add(UGraphNodeRegistrar::StaticClass()->GetClassPathName());
	RegistrarFilter.bRecursiveClasses = true;

	TArray<FAssetData> FoundRegistrarAssets;
	AssetRegistry.GetAssets(RegistrarFilter, FoundRegistrarAssets);

	KnownRegistrars.Append(FoundRegistrarAssets);

	UE_LOG(LogHeartNodeRegistry, Log, TEXT("FetchAssetRegistrars found '%i' registrars"), FoundRegistrarAssets.Num())

	RefreshAssetRegistrars();
}

void UHeartRegistryRuntimeSubsystem::RefreshAssetRegistrars(const bool ForceRefresh)
{
	QUICK_SCOPE_CYCLE_COUNTER(RefreshAssetRegistrars)

	UE_LOG(LogHeartNodeRegistry, Log, TEXT("RefreshAssetRegistrars: '%i' known registrars"), KnownRegistrars.Num())

	for (const FAssetData& RegistrarAsset : KnownRegistrars)
	{
		if (auto&& Registrar = Cast<UGraphNodeRegistrar>(RegistrarAsset.GetAsset()))
		{
			if (ForceRefresh)
			{
				AutoRemoveRegistrar(Registrar);
			}

			AutoAddRegistrar(Registrar);
		}
	}
}

UHeartGraphNodeRegistry* UHeartRegistryRuntimeSubsystem::GetRegistry_Internal(const TSubclassOf<UHeartGraphSchema>& Class)
{
	check(Class);

	const FSoftClassPath Path(Class);

	if (auto&& FoundRegistry = Registries.Find(Path))
	{
		if (ensure(IsValid(*FoundRegistry)))
		{
			return *FoundRegistry;
		}
	}

	const UClass* RegistryClass = GetDefault<UHeartGraphSchema>(Class)->GetRegistryClass();

	if (!IsValid(RegistryClass))
	{
		UE_LOG(LogHeartNodeRegistry, Warning, TEXT("GetRegistryClass returned invalid class for Graph '%s'!"), *Class->GetName())
		RegistryClass = UHeartGraphNodeRegistry::StaticClass();
	}

	auto&& NewRegistry = NewObject<UHeartGraphNodeRegistry>(this, RegistryClass);
	Registries.Add(Path, NewRegistry);
	BroadcastPostRegistryAdded(NewRegistry);

	NewRegistry->GetOnRegistryChangedNative().AddUObject(this, &ThisClass::OnRegistryChanged);

	return NewRegistry;
}

UHeartGraphNodeRegistry* UHeartRegistryRuntimeSubsystem::GetRegistry_Internal(const TSubclassOf<UHeartGraph>& Class)
{
	check(Class);

	const FSoftClassPath Path(Class);

	TSubclassOf<UHeartGraphSchema> SchemaClass = GetDefault<UHeartGraph>(Class)->GetSchema()->GetClass();

	return GetRegistry_Internal(SchemaClass);
}

void UHeartRegistryRuntimeSubsystem::OnRegistryChanged(UHeartGraphNodeRegistry* Registry)
{
	BroadcastOnAnyRegistryChanged(Registry);
}

void UHeartRegistryRuntimeSubsystem::AutoAddRegistrar(const UGraphNodeRegistrar* Registrar)
{
	if (!Registrar->ShouldRegister())
	{
		return;
	}

	for (auto&& SchemaPath : Registrar->AutoRegisterTo)
	{
		if (const TSubclassOf<UHeartGraphSchema> Class = SchemaPath.TryLoadClass<UHeartGraphSchema>())
		{
			auto&& Registry = GetRegistry_Internal(Class);
			check(Registry);
			if (!Registry->IsRegistered(Registrar))
			{
				Registry->AddRegistrar(Registrar);
			}
		}
	}

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	for (auto&& ClassPath : Registrar->AutoRegisterWith)
	{
		if (const TSubclassOf<UHeartGraph> Class = ClassPath.TryLoadClass<UHeartGraph>())
		{
			auto&& Registry = GetRegistry_Internal(Class);
			check(Registry);
			if (!Registry->IsRegistered(Registrar))
			{
				Registry->AddRegistrar(Registrar);
			}
		}
	}
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

void UHeartRegistryRuntimeSubsystem::AutoRemoveRegistrar(UGraphNodeRegistrar* Registrar)
{
	for (auto&& ClassPath : Registrar->AutoRegisterTo)
	{
		if (const TSubclassOf<UHeartGraphSchema> Class = ClassPath.TryLoadClass<UHeartGraphSchema>())
		{
			GetRegistry_Internal(Class)->RemoveRegistrar(Registrar);
		}
	}

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	for (auto&& ClassPath : Registrar->AutoRegisterWith)
	{
		if (const TSubclassOf<UHeartGraph> Class = ClassPath.TryLoadClass<UHeartGraph>())
		{
			GetRegistry_Internal(Class)->RemoveRegistrar(Registrar);
		}
	}
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

void UHeartRegistryRuntimeSubsystem::BroadcastPostRegistryAdded(UHeartGraphNodeRegistry* Registry)
{
	PostRegistryAddedNative.Broadcast(Registry);
	{
#if WITH_EDITOR
		FEditorScriptExecutionGuard EditorScriptExecutionGuard;
#endif
		PostRegistryAdded.Broadcast(Registry);
	}
}

void UHeartRegistryRuntimeSubsystem::BroadcastPreRegistryRemoved(UHeartGraphNodeRegistry* Registry)
{
	PreRegistryRemovedNative.Broadcast(Registry);
	{
#if WITH_EDITOR
		FEditorScriptExecutionGuard EditorScriptExecutionGuard;
#endif
		PreRegistryRemoved.Broadcast(Registry);
	}
}

void UHeartRegistryRuntimeSubsystem::BroadcastOnAnyRegistryChanged(UHeartGraphNodeRegistry* Registry)
{
	OnAnyRegistryChangedNative.Broadcast(Registry);
	{
#if WITH_EDITOR
		FEditorScriptExecutionGuard EditorScriptExecutionGuard;
#endif
		OnAnyRegistryChanged.Broadcast(Registry);
	}
}

UHeartGraphNodeRegistry* UHeartRegistryRuntimeSubsystem::GetNodeRegistry(const TSubclassOf<UHeartGraphSchema> Class)
{
	if (IsValid(Class))
	{
		return GetRegistry_Internal(Class);
	}
	return nullptr;
}

UHeartGraphNodeRegistry* UHeartRegistryRuntimeSubsystem::GetNodeRegistryForGraph(const UHeartGraph* Graph)
{
	if (!IsValid(Graph) || !Graph->GetSchema()) return nullptr;
	UClass* Class = Graph->GetSchema()->GetClass();
	return GetNodeRegistry(Class);
}

void UHeartRegistryRuntimeSubsystem::AddToRegistry(UGraphNodeRegistrar* Registrar, const TSubclassOf<UHeartGraphSchema> To)
{
	GetNodeRegistry(To)->AddRegistrar(Registrar);
}

void UHeartRegistryRuntimeSubsystem::RemoveFromRegistry(UGraphNodeRegistrar* Registrar, const TSubclassOf<UHeartGraphSchema> From)
{
	GetNodeRegistry(From)->RemoveRegistrar(Registrar);
}

UHeartGraphNodeRegistry* UHeartRegistryRuntimeSubsystem::GetRegistry(const TSubclassOf<UHeartGraph> Class)
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	return GetRegistry_Internal(Class);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

void UHeartRegistryRuntimeSubsystem::AddRegistrar(UGraphNodeRegistrar* Registrar, const TSubclassOf<UHeartGraph> To)
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	GetRegistry(To)->AddRegistrar(Registrar);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

void UHeartRegistryRuntimeSubsystem::RemoveRegistrar(UGraphNodeRegistrar* Registrar, const TSubclassOf<UHeartGraph> From)
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	GetRegistry(From)->RemoveRegistrar(Registrar);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}