// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "HeartRegistrationClasses.h"
#include "Templates/SubclassOf.h"

#include "HeartGraphNodeRegistry.h"
#include "Model/HeartGraph.h"

#include "HeartNodeRegistrySubsystem.generated.h"

class UGraphNodeRegistrar;

DECLARE_LOG_CATEGORY_EXTERN(LogHeartNodeRegistry, Log, All);

/**
 * Global singleton that stores which graphs can use which registries. May optionally exist at runtime, so always check
 * for validity before using the pointer returned by GetEngineSubsystem
 */
UCLASS()
class HEART_API UHeartNodeRegistrySubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

#if WITH_EDITOR
	void SetupBlueprintCaching();

	void OnFilesLoaded();
	void OnAssetAdded(const FAssetData& AssetData);
	void OnAssetRemoved(const FAssetData& AssetData);
	void OnHotReload(EReloadCompleteReason ReloadCompleteReason);
	void OnBlueprintPreCompile(UBlueprint* Blueprint);
	void OnBlueprintCompiled();
#endif

protected:
	UBlueprint* GetNodeBlueprint(const FAssetData& AssetData);

	void FetchNativeClasses();
	void FetchAssetRegistryAssets();

	UHeartGraphNodeRegistry* GetRegistry_Internal(const FSoftClassPath ClassPath);

public:
	void AutoAddRegistrar(UGraphNodeRegistrar* Registrar);
	void AutoRemoveRegistrar(UGraphNodeRegistrar* Registrar);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeRegistrySubsystem")
	UHeartGraphNodeRegistry* GetRegistry(const TSubclassOf<UHeartGraph> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeRegistrySubsystem")
	void AddRegistrar(UGraphNodeRegistrar* Registrar, TSubclassOf<UHeartGraph> To);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeRegistrySubsystem")
	void RemoveRegistrar(UGraphNodeRegistrar* Registrar, TSubclassOf<UHeartGraph> From);

	UGraphNodeRegistrar* GetFallbackRegistrar() const { return FallbackRegistrar; }

private:
	// Maps Classes to the Registry instance we keep for them
	UPROPERTY()
	TMap<FSoftClassPath, TObjectPtr<UHeartGraphNodeRegistry>> NodeRegistries;

	UPROPERTY()
	TObjectPtr<UGraphNodeRegistrar> FallbackRegistrar;

	FHeartRegistrationClasses KnownNativeClasses;

#if WITH_EDITOR
	int32 WaitingForBlueprintToCompile;
	TMap<FName, FAssetData> KnownBlueprintHeartGraphNodes;
	bool HasSetupBlueprintCaching = false;
#endif
};
