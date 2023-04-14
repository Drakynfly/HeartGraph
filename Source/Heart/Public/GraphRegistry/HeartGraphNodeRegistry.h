// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartRegistrationClasses.h"

#include "HeartGraphNodeRegistry.generated.h"

DECLARE_DELEGATE_RetVal_OneParam(bool, FNativeNodeClassFilter, UClass* /* Class*/);

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FNodeClassFilter, UClass*, Class);

class UHeartGraphNode;
class UHeartGraphPin;
class UGraphNodeRegistrar;

/**
 * Stores a list of nodes, graph nodes, usable by a Graph, along with their internal graph representations and
 * visual representations.
 * The registry counts how many times each class is registered so that multiple registrars can safely register the same
 * classes and they can be deregistered without removing classes that other registrars have also added.
 */
UCLASS()
class HEART_API UHeartGraphNodeRegistry : public UObject
{
	GENERATED_BODY()

	friend class UHeartNodeRegistrySubsystem;

protected:
	bool FilterClassForRegistration(const TObjectPtr<UClass>& Class) const;

	void AddRegistrationList(const FHeartRegistrationClasses& Registration);
	void RemoveRegistrationList(const FHeartRegistrationClasses& Registration);

	void SetRecursivelyDiscoveredClasses(const FHeartRegistrationClasses& Classes);

	FHeartRegistrationClasses GetClassesRegisteredRecursively();

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	TArray<FString> GetNodeCategories() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void GetNodeClasses(TArray<UClass*>& OutClasses) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void GetNodeClassesWithGraphClass(TMap<UClass*, TSubclassOf<UHeartGraphNode>>& OutClasses) const;

	void GetFilteredNodeClasses(const FNativeNodeClassFilter& Filter, TArray<UClass*>& OutClasses) const;

	void GetFilteredNodeClassesWithGraphClass(const FNativeNodeClassFilter& Filter, TMap<UClass*, TSubclassOf<UHeartGraphNode>>& OutClasses) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void GetFilteredNodeClasses(const FNodeClassFilter& Filter, TArray<UClass*>& OutClasses) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void GetFilteredNodeClassesWithGraphClass(const FNodeClassFilter& Filter, TMap<UClass*, TSubclassOf<UHeartGraphNode>>& OutClasses) const;


	/**
	 * Get the graph node class that we use to represent the given arbitrary class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	TSubclassOf<UHeartGraphNode> GetGraphNodeClassForNode(const UClass* NodeClass) const;

	/**
	 * Get the visualizer class that we use to represent the given node class.
	 * If VisualizerBase is non-null, will only return a visualizer that inherits from that class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry", meta = (DeterminesOutputType = "VisualizerBase"))
	UClass* GetVisualizerClassForGraphNode(TSubclassOf<UHeartGraphNode> GraphNodeClass, UClass* VisualizerBase = nullptr) const;

	/**
	 * Get the visualizer class that we use to represent the given pin class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry", meta = (DeterminesOutputType = "VisualizerBase"))
	UClass* GetVisualizerClassForGraphPin(TSubclassOf<UHeartGraphPin> GraphPinClass, UClass* VisualizerBase = nullptr) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry", meta = (DeterminesOutputType = "VisualizerBase"))
	UClass* GetVisualizerClassForGraphConnection(TSubclassOf<UHeartGraphPin> FromPinClass, TSubclassOf<UHeartGraphPin> ToPinClass, UClass* VisualizerBase = nullptr) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	void AddRegistrar(UGraphNodeRegistrar* Registrar);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	void RemoveRegistrar(UGraphNodeRegistrar* Registrar);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	void DeregisterAll();

private:
	TMap<TSubclassOf<UHeartGraphNode>, TMap<TObjectPtr<UClass>, int32>> GraphClasses;

	// Maps Graph Node classes to the visualizer class that can represent them in an interactive graph.
	TMap<TSubclassOf<UHeartGraphNode>, TMap<TObjectPtr<UClass>, int32>> NodeVisualizerMap;

	// Maps Graph Pin classes to the visualizer class that can represent them in an interactive graph.
	TMap<TSubclassOf<UHeartGraphPin>, TMap<TObjectPtr<UClass>, int32>> PinVisualizerMap;

	// We have to store these hard-ref'd to keep around the stuff in GraphClasses as we cannot UPROP TMaps of TMaps
	UPROPERTY()
	TArray<TObjectPtr<UGraphNodeRegistrar>> ContainedRegistrars;

	UPROPERTY()
	FHeartRegistrationClasses RecursivelyDiscoveredClasses;
};