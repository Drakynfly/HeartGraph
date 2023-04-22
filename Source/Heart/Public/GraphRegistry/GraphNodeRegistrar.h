// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "HeartRegistrationClasses.h"
#include "GraphNodeRegistrar.generated.h"

/**
 * A list of classes to register to a HeartGraphNodeRegistry
 */
UCLASS()
class HEART_API UGraphNodeRegistrar : public UPrimaryDataAsset
{
	GENERATED_BODY()

	friend class UHeartGraphNodeRegistry;
	friend class UHeartRegistryRuntimeSubsystem;

#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PreEditChange(FEditPropertyChain& PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

protected:
    // Registration class arrays
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FHeartRegistrationClasses Registration;

	// Classes to register this registrar for automatically. If this is empty, it must be manually added to the Registry
	// with UHeartGraphNodeRegistry::AddRegistrar
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MetaClass = "/Script/Heart.HeartGraph"))
	TArray<FSoftClassPath> AutoRegisterWith;

	/**
	 * Are the classes we register only registering themselves, or all their children as well. This must be enabled
	 * when registering abstract classes.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Recursive;
};
