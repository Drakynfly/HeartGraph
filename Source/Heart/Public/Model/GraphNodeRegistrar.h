// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "HeartRegistrationClasses.h"
#include "GraphNodeRegistrar.generated.h"

class UHeartGraph;
class UHeartGraphNode;

/**
 *
 */
UCLASS()
class HEART_API UGraphNodeRegistrar : public UPrimaryDataAsset
{
	GENERATED_BODY()

	friend class UHeartGraphNodeRegistry;
	friend class UHeartNodeRegistrySubsystem;

#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// Graph classes to register these nodes for
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UHeartGraph>> RegisterWith;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FHeartRegistrationClasses Registration;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowAbstract = false))
	TArray<TSubclassOf<UClass>> NodeClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowAbstract = false))
	TArray<TSubclassOf<UHeartGraphNode>> GraphNodeClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowAbstract = false, MustImplement = "/Script/Heart.GraphNodeVisualizerInterface"))
	TArray<TObjectPtr<UClass>> NodeVisualizerClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowAbstract = false, MustImplement = "/Script/Heart.GraphPinVisualizerInterface"))
	TArray<TObjectPtr<UClass>> PinVisualizerClasses;


	/**
	 * Should this registrar allow itself to be automatically discovered by the Registry Subsystem. If this is disabled,
	 * it must be manually added with UHeartNodeRegistrySubsystem::AddRegistrar
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool AutoRegister = true;

	/**
	 * Are the classes we register only registering themselves, or all their children as well. This must be enabled
	 * when registering abstract classes.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Recursive;
};
