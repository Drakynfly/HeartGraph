// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
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

protected:
	// Graph classes to register these nodes for
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UHeartGraph>> RegisterWith;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowAbstract = false))
	TArray<TObjectPtr<UClass>> NodeClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowAbstract = false))
	TArray<TSubclassOf<UHeartGraphNode>> GraphNodeClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowAbstract = false, MustImplement = "GraphNodeVisualizerInterface"))
	TArray<TObjectPtr<UClass>> NodeVisualizerClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowAbstract = false, MustImplement = "GraphPinVisualizerInterface"))
	TArray<TObjectPtr<UClass>> PinVisualizerClasses;
};
