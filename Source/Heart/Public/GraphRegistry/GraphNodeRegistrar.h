// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "HeartRegistryStructs.h"
#include "GraphNodeRegistrar.generated.h"

class UHeartGraphNodeRegistry;

/**
 * A list of classes to register to a HeartGraphNodeRegistry
 */
UCLASS(Const)
class HEART_API UGraphNodeRegistrar : public UPrimaryDataAsset
{
	GENERATED_BODY()

	friend class UHeartRegistryRuntimeSubsystem;

public:
#if WITH_EDITOR
	virtual void PostLoad() override;
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PreEditChange(FEditPropertyChain& PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

	UE_DEPRECATED(5.5, "use ClassLists instead (switching to soft classes)")
	const FHeartRegistrationClasses& GetRegistrationList() const { return Registration; }

	const FHeartRegistryClassLists& GetClassLists() const { return ClassLists; }

	virtual bool ShouldRegister() const;

	virtual void OnRegistered(UHeartGraphNodeRegistry* Registry) const;
	virtual void OnDeregistered(UHeartGraphNodeRegistry* Registry) const;

protected:
	void RegisterAssets(UHeartGraphNodeRegistry* Registry) const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "Register"))
	void BP_Register(UHeartGraphNodeRegistry* Registry) const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "Deregister"))
	void BP_Deregister(UHeartGraphNodeRegistry* Registry) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GraphNodeRegistrar", meta = (ShowOnlyInnerProperties))
	FHeartRegistryClassLists ClassLists;

	UE_DEPRECATED(5.5, "use ClassLists instead (switching to soft classes)")
	UPROPERTY()
	FHeartRegistrationClasses Registration;

	// Classes to register this registrar for automatically. If this is empty, it must be manually added to the Registry
	// with UHeartGraphNodeRegistry::AddRegistrar
	UE_DEPRECATED(5.5, "use AutoRegisterTo instead (per Schema registration)")
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (MetaClass = "/Script/Heart.HeartGraph"))
	TArray<FSoftClassPath> AutoRegisterWith;

	// Schemas to register this registrar for automatically. If this is empty, it must be manually added to the Registry
	// with UHeartGraphNodeRegistry::AddRegistrar
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MetaClass = "/Script/Heart.HeartGraphSchema"))
	TArray<FSoftClassPath> AutoRegisterTo;
};