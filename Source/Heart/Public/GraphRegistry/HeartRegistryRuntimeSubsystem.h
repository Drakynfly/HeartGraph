// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"

#include "Templates/SubclassOf.h"

#include "HeartGraphNodeRegistry.h"
#include "Model/HeartGraph.h"

#include "HeartRegistryRuntimeSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHeartNodeRegistry, Log, All);

class UHeartGraphNodeRegistry;
DECLARE_MULTICAST_DELEGATE_OneParam(FHeartRegistryEventNative, UHeartGraphNodeRegistry*);
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
	friend class UHeartRegistryEditorSubsystem;

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void LoadFallbackRegistrar();

	FHeartRegistryEventNative& GetPostRegistryAddedNative() { return PostRegistryAddedNative; }
	FHeartRegistryEventNative& GetPreRegistryRemovedNative() { return PreRegistryRemovedNative; }
	FHeartRegistryEventNative& GetOnAnyRegistryChangedNative() { return OnAnyRegistryChangedNative; }

protected:
	void OnFilesLoaded();
	void OnAssetAdded(const FAssetData& AssetData);
	void OnAssetRemoved(const FAssetData& AssetData);

	void FetchAssetRegistryAssets();

	UHeartGraphNodeRegistry* GetRegistry_Internal(const TSubclassOf<UHeartGraph> Class);

	void OnRegistryChanged(UHeartGraphNodeRegistry* Registry);

	UGraphNodeRegistrar* GetFallbackRegistrar() const { return FallbackRegistrar; }

	void AutoAddRegistrar(UGraphNodeRegistrar* Registrar);
	void AutoRemoveRegistrar(UGraphNodeRegistrar* Registrar);

	void BroadcastPostRegistryAdded(UHeartGraphNodeRegistry* Registry);
	void BroadcastPreRegistryRemoved(UHeartGraphNodeRegistry* Registry);
	void BroadcastOnAnyRegistryChanged(UHeartGraphNodeRegistry* Registry);

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|RuntimeRegistry")
	UHeartGraphNodeRegistry* GetRegistry(const TSubclassOf<UHeartGraph> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|RuntimeRegistry")
	void AddRegistrar(UGraphNodeRegistrar* Registrar, TSubclassOf<UHeartGraph> To);

	UFUNCTION(BlueprintCallable, Category = "Heart|RuntimeRegistry")
	void RemoveRegistrar(UGraphNodeRegistrar* Registrar, TSubclassOf<UHeartGraph> From);

	UPROPERTY(BlueprintAssignable, Category = "Heart|RuntimeRegistry|Events")
	FHeartRegistryEvent PostRegistryAdded;

	UPROPERTY(BlueprintAssignable, Category = "Heart|RuntimeRegistry|Events")
	FHeartRegistryEvent PreRegistryRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Heart|RuntimeRegistry|Events")
	FHeartRegistryEvent OnAnyRegistryChanged;

protected:
	FHeartRegistryEventNative PostRegistryAddedNative;
	FHeartRegistryEventNative PreRegistryRemovedNative; // Future-proofing: Currently, Registries are never removed, but if they are, use this.
	FHeartRegistryEventNative OnAnyRegistryChangedNative;

private:
	// Maps Classes to the Registry instance we keep for them
	UPROPERTY()
	TMap<FSoftClassPath, TObjectPtr<UHeartGraphNodeRegistry>> Registries;

	UPROPERTY()
	TObjectPtr<UGraphNodeRegistrar> FallbackRegistrar;
};
