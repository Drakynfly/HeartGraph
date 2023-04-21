// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "HeartRegistrationClasses.h"
#include "Templates/SubclassOf.h"

#include "HeartGraphNodeRegistry.h"
#include "Model/HeartGraph.h"

#include "HeartRegistryRuntimeSubsystem.generated.h"

class UGraphNodeRegistrar;

DECLARE_LOG_CATEGORY_EXTERN(LogHeartNodeRegistry, Log, All);

/**
 * Global singleton that stores a node registry for each class of Heart Graph. Runtime existence is optional, so always
 * check for validity before using the pointer returned by GetEngineSubsystem
 */
UCLASS()
class HEART_API UHeartRegistryRuntimeSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

	friend class UHeartRegistryEditorSubsystem;

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

protected:
	void FetchNativeClasses();
	void FetchAssetRegistryAssets();

	void FindRecursiveClassesForRegistry(UHeartGraphNodeRegistry* Registry);

	UHeartGraphNodeRegistry* GetRegistry_Internal(const FSoftClassPath& ClassPath);

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
	TMap<FSoftClassPath, TObjectPtr<UHeartGraphNodeRegistry>> Registries;

	UPROPERTY()
	TObjectPtr<UGraphNodeRegistrar> FallbackRegistrar;

	FHeartRegistrationClasses KnownNativeClasses;
};
