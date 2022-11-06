// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "HeartGraph.h"
#include "HeartNodeRegistrySubsystem.generated.h"

class UHeartGraphNode;
class UHeartGraphPin;
class UGraphNodeRegistrar;

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FNodeClassFilter, UClass*, Class);

UCLASS()
class HEART_API UHeartGraphNodeRegistry : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void GetNodeClasses(TArray<UClass*>& OutClasses) const { OutClasses = NodeClasses.Array(); }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void GetFilteredNodeClasses(const FNodeClassFilter& Filter, TArray<UClass*>& OutClasses) const;

	/**
	 * Get the graph node class that we use to represent the given arbitrary class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	UClass* GetGraphNodeClassForNode(UClass* NodeClass) const;

	/**
	 * Get the visualizer class that we use to represent the given node class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	UClass* GetVisualizerClassForGraphNode(TSubclassOf<UHeartGraphNode> GraphNodeClass) const;

	/**
	* Get the visualizer class that we use to represent the given pin class.
	*/
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	UClass* GetVisualizerClassForGraphPin(TSubclassOf<UHeartGraphPin> GraphPinClass) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	void AddRegistrar(UGraphNodeRegistrar* Registrar);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	void RemoveRegistrar(UGraphNodeRegistrar* Registrar);

private:
	UPROPERTY()
	TSet<TObjectPtr<UClass>> NodeClasses;

	// Maps classes to the Graph Node class that can represent them in a graph.
	UPROPERTY()
	TMap<TObjectPtr<UClass>, TSubclassOf<UHeartGraphNode>> GraphNodeMap;

	// Maps Graph Node classes to the visualizer class that can represent them in an interactive graph.
	UPROPERTY()
	TMap<TSubclassOf<UHeartGraphNode>, TObjectPtr<UClass>> NodeVisualizerMap;

	// Maps Graph Pin classes to the visualizer class that can represent them in an interactive graph.
	UPROPERTY()
	TMap<TSubclassOf<UHeartGraphPin>, TObjectPtr<UClass>> PinVisualizerMap;
};

/**
 * Global singleton that stores which graphs can use which registries.
 */
UCLASS()
class HEART_API UHeartNodeRegistrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|NodeRegistrySubsystem")
	UHeartGraphNodeRegistry* GetRegistry(const TSubclassOf<UHeartGraph> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeRegistrySubsystem")
	void AddRegistrar(UGraphNodeRegistrar* Registrar);

	UFUNCTION(BlueprintCallable, Category = "Heart|NodeRegistrySubsystem")
	void RemoveRegistrar(UGraphNodeRegistrar* Registrar);

private:
	UPROPERTY()
	TMap<TSubclassOf<UHeartGraph>, TObjectPtr<UHeartGraphNodeRegistry>> NodeRegistries;
};
