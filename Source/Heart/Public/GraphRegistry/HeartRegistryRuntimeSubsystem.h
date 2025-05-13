// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"

#include "Templates/SubclassOf.h"

#include "HeartRegistryRuntimeSubsystem.generated.h"

class UHeartGraph;
class UHeartGraphSchema;
HEART_API DECLARE_LOG_CATEGORY_EXTERN(LogHeartNodeRegistry, Log, All);

class UHeartGraphNodeRegistry;
using FHeartRegistryEventNative = TMulticastDelegate<void(UHeartGraphNodeRegistry*)>;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeartRegistryEvent, class UHeartGraphNodeRegistry*, Registry);

/**
 * Global singleton that stores a node registry for each class of Heart Graph. Runtime existence is optional, so always
 * check for validity before using the pointer returned by GetEngineSubsystem
 */
UCLASS()
class HEART_API UHeartRegistryRuntimeSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

	friend class UGraphNodeRegistrar;
	friend class UHeartGraphNodeRegistry;
	friend class UHeartGraphSettings;
	friend class UHeartRegistryEditorSubsystem;

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	FHeartRegistryEventNative::RegistrationType& GetPostRegistryAddedNative() { return PostRegistryAddedNative; }
	FHeartRegistryEventNative::RegistrationType& GetPreRegistryRemovedNative() { return PreRegistryRemovedNative; }
	FHeartRegistryEventNative::RegistrationType& GetOnAnyRegistryChangedNative() { return OnAnyRegistryChangedNative; }

protected:
	void OnFilesLoaded();
	void OnAssetAdded(const FAssetData& AssetData);
	void OnAssetRemoved(const FAssetData& AssetData);

	void LoadFallbackRegistrar();

	// Search for Registrars defined in code. This is called once on startup of this subsystem.
	void FetchNativeRegistrars();

	// Search for Registrars defined as BP assets. The editor calls this occasionally to reload registrars as they
	// are edited.
	void FetchAssetRegistrars();

	void RefreshAssetRegistrars(bool ForceRefresh = false);

	UHeartGraphNodeRegistry* GetRegistry_Internal(const TSubclassOf<UHeartGraphSchema>& Class);

	UE_DEPRECATED(5.5, "Use the overload that takes a Schema class")
	UHeartGraphNodeRegistry* GetRegistry_Internal(const TSubclassOf<UHeartGraph>& Class);

	void OnRegistryChanged(UHeartGraphNodeRegistry* Registry);

	UGraphNodeRegistrar* GetFallbackRegistrar() const { return FallbackRegistrar; }

	// Add a registrar to every registry for the classes that the register lists in AutoRegisterWith
	void AutoAddRegistrar(const UGraphNodeRegistrar* Registrar);

	// Remove a registrar from every registry for the classes that the register lists in AutoRegisterWith
	void AutoRemoveRegistrar(UGraphNodeRegistrar* Registrar);

	void BroadcastPostRegistryAdded(UHeartGraphNodeRegistry* Registry);
	void BroadcastPreRegistryRemoved(UHeartGraphNodeRegistry* Registry);
	void BroadcastOnAnyRegistryChanged(UHeartGraphNodeRegistry* Registry);

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|RuntimeRegistry")
	UHeartGraphNodeRegistry* GetNodeRegistry(const TSubclassOf<UHeartGraphSchema> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|RuntimeRegistry")
	UHeartGraphNodeRegistry* GetNodeRegistryForGraph(const UHeartGraph* Graph);

	UFUNCTION(BlueprintCallable, Category = "Heart|RuntimeRegistry")
	void AddToRegistry(UGraphNodeRegistrar* Registrar, TSubclassOf<UHeartGraphSchema> To);

	UFUNCTION(BlueprintCallable, Category = "Heart|RuntimeRegistry")
	void RemoveFromRegistry(UGraphNodeRegistrar* Registrar, TSubclassOf<UHeartGraphSchema> From);

	UE_DEPRECATED(5.5, "Use GetNodeRegistry or GetNodeRegistryForGraph instead (with schema class)")
	UFUNCTION(BlueprintCallable, Category = "Heart|RuntimeRegistry")
	UHeartGraphNodeRegistry* GetRegistry(const TSubclassOf<UHeartGraph> Class);

	UE_DEPRECATED(5.5, "Use AddToRegistry instead (with schema class)")
	UFUNCTION(BlueprintCallable, Category = "Heart|RuntimeRegistry")
	void AddRegistrar(UGraphNodeRegistrar* Registrar, TSubclassOf<UHeartGraph> To);

	UE_DEPRECATED(5.5, "Use RemoveFromRegistry instead (with schema class)")
	UFUNCTION(BlueprintCallable, Category = "Heart|RuntimeRegistry")
	void RemoveRegistrar(UGraphNodeRegistrar* Registrar, TSubclassOf<UHeartGraph> From);

protected:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FHeartRegistryEvent PostRegistryAdded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FHeartRegistryEvent PreRegistryRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FHeartRegistryEvent OnAnyRegistryChanged;

	FHeartRegistryEventNative PostRegistryAddedNative;
	FHeartRegistryEventNative PreRegistryRemovedNative; // Future-proofing: Currently, Registries are never removed, but if they are, use this.
	FHeartRegistryEventNative OnAnyRegistryChangedNative;

private:
	// Maps Schema Classes to the Registry instance we keep for them
	UPROPERTY()
	TMap<FSoftClassPath, TObjectPtr<UHeartGraphNodeRegistry>> Registries;

	UPROPERTY()
	TObjectPtr<UGraphNodeRegistrar> FallbackRegistrar;

	TSet<FAssetData> KnownRegistrars;
};