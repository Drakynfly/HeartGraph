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

USTRUCT()
struct FRefCountedBase
{
	GENERATED_BODY()

	void operator++() { RefCount++; }

	void operator--() { RefCount--; }

	uint32 GetRefCount() const { return RefCount; }

private:
	UPROPERTY()
	uint32 RefCount = 0;
};

USTRUCT()
struct FRefCountedClass : public FRefCountedBase
{
	GENERATED_BODY()

	FRefCountedClass() {}

	FRefCountedClass(UClass* Class)
	  : Class(Class) {}

	UPROPERTY()
	TObjectPtr<UClass> Class;

	friend bool operator==(const FRefCountedClass& Lhs, const FRefCountedClass& RHS)
	{
		return Lhs.Class == RHS.Class;
	}

	friend bool operator!=(const FRefCountedClass& Lhs, const FRefCountedClass& RHS)
	{
		return !(Lhs == RHS);
	}
};

FORCEINLINE uint32 GetTypeHash(const FRefCountedClass& RefCountedClass)
{
	uint32 KeyHash = 0;
	KeyHash = HashCombine(KeyHash, GetTypeHash(RefCountedClass.GetRefCount()));
	KeyHash = HashCombine(KeyHash, GetTypeHash(RefCountedClass.Class));
	return KeyHash;
}

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

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	void DeregisterAll();

private:
	TMap<TSubclassOf<UHeartGraphNode>, TMap<TObjectPtr<UClass>, int32>> GraphClasses;

	// Maps Graph Node classes to the visualizer class that can represent them in an interactive graph.
	UPROPERTY()
	TMap<TSubclassOf<UHeartGraphNode>, FRefCountedClass> NodeVisualizerMap;

	// Maps Graph Pin classes to the visualizer class that can represent them in an interactive graph.
	UPROPERTY()
	TMap<TSubclassOf<UHeartGraphPin>, FRefCountedClass> PinVisualizerMap;

	// We have to store these hard-ref'd to keep around the stuff in GraphClasses as we cannot UPROP TMultiMaps
	UPROPERTY()
	TArray<TObjectPtr<UGraphNodeRegistrar>> ContainedRegistrars;

	UPROPERTY()
	FHeartRegistrationClasses RecursivelyDiscoveredClasses;
};