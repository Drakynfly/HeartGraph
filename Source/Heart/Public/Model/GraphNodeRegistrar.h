// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GraphNodeRegistrar.generated.h"

class UHeartGraphBehavior;
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
	// Behavior classes to register these nodes for
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UHeartGraphBehavior>> RegisterWith;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowAbstract = false))
	TArray<TObjectPtr<UClass>> NodeClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowAbstract = false))
	TArray<TSubclassOf<UHeartGraphNode>> GraphNodeClasses;
};
