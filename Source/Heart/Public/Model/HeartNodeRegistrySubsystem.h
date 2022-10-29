// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "HeartGraph.h"
#include "HeartNodeRegistrySubsystem.generated.h"

class UHeartGraphNode;
class UGraphNodeRegistrar;

UCLASS()
class HEART_API UHeartGraphNodeRegistry : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphBehavior")
	TArray<UClass*> GetNodeClasses() const { return NodeClasses.Array(); }

	/**
	 * Get the graph node class that we use to represent the given arbitrary class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphBehavior")
	TSubclassOf<UHeartGraphNode> GetGraphNodeClassForNode(UClass* NodeClass) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphBehavior")
	void AddRegistrar(UGraphNodeRegistrar* Registrar);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphBehavior")
	void RemoveRegistrar(UGraphNodeRegistrar* Registrar);

private:
	UPROPERTY()
	TSet<TObjectPtr<UClass>> NodeClasses;

	// Maps classes to the Graph Node class that can represent them in a graph.
	UPROPERTY()
	TMap<TObjectPtr<UClass>, TSubclassOf<UHeartGraphNode>> GraphNodeMap;
};

/**
 * Global singleton that stores which graphs can use which registries.
 */
UCLASS()
class HEART_API UHeartNodeRegistrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|Node Registry Subsystem")
	UHeartGraphNodeRegistry* GetRegistry(const TSubclassOf<UHeartGraph> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|Node Registry Subsystem")
	void AddRegistrar(UGraphNodeRegistrar* Registrar);

	UFUNCTION(BlueprintCallable, Category = "Heart|Node Registry Subsystem")
	void RemoveRegistrar(UGraphNodeRegistrar* Registrar);

private:
	UPROPERTY()
	TMap<TSubclassOf<UHeartGraph>, TObjectPtr<UHeartGraphNodeRegistry>> NodeRegistries;
};
