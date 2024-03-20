// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartNodeSource.h"
#include "HeartRegistrationClasses.h"
#include "General/CountedPtr.h"
#include "Model/HeartGraphPinTag.h"

#include "HeartGraphNodeRegistry.generated.h"

namespace Heart::Query
{
	class FRegistryQueryResult;
}

struct FHeartGraphPinDesc;
class UHeartGraphNode;
class UGraphNodeRegistrar;

class UHeartGraphNodeRegistry;
using FHeartGraphNodeRegistryEventNative = TMulticastDelegate<void(UHeartGraphNodeRegistry*)>;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeartGraphNodeRegistryEvent, UHeartGraphNodeRegistry*, Registry);


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

	// Registrar is allowed to call AddRegistrationList/RemoveRegistrationList
	friend class UGraphNodeRegistrar;
	friend Heart::Query::FRegistryQueryResult;

protected:
	bool FilterObjectForRegistration(const UObject* Object) const;

	void AddRegistrationList(const FHeartRegistrationClasses& Registration, bool Broadcast = true);
	void RemoveRegistrationList(const FHeartRegistrationClasses& Registration, bool Broadcast = true);

	void BroadcastChange();

public:
	FHeartGraphNodeRegistryEventNative& GetOnRegistryChangedNative() { return OnRegistryChangedNative; }

	Heart::Query::FRegistryQueryResult QueryRegistry() const;

	void ForEachNodeObjectClass(const TFunctionRef<bool(const FHeartNodeArchetype&)>& Iter) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	bool IsRegistered(const UGraphNodeRegistrar* Registrar) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	int32 GetNumNodes(bool IncludeRecursive) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	TArray<FString> GetNodeCategories() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void GetAllNodeSources(TArray<FHeartNodeSource>& OutNodeSources) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void GetAllGraphNodeArchetypes(TArray<FHeartNodeArchetype>& OutArchetypes) const;

	/**
	 * Get the graph node class that we use to represent the given arbitrary class.
	 */
	UE_DEPRECATED(5.4, "Please use GetGraphNodeClassesForNode or another method of determining a NodeSource's graph node")
	UFUNCTION(BlueprintCallable)
	virtual TSubclassOf<UHeartGraphNode> GetGraphNodeClassForNode(const FHeartNodeSource NodeSource) const;

	/**
	 * Get the graph node classes that we can use to represent the given arbitrary class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	virtual TArray<TSubclassOf<UHeartGraphNode>> GetGraphNodeClassesForNode(const FHeartNodeSource NodeSource) const;

	/**
	 * Get the visualizer class that we use to represent the given node class.
	 * If VisualizerBase is non-null, will only return a visualizer that inherits from that class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry", meta = (DeterminesOutputType = "VisualizerBase"))
	virtual UClass* GetVisualizerClassForGraphNode(TSubclassOf<UHeartGraphNode> GraphNodeClass, UClass* VisualizerBase = nullptr) const;

	/**
	 * Get the visualizer class that we use to represent the given pin class.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry", meta = (DeterminesOutputType = "VisualizerBase"))
	virtual UClass* GetVisualizerClassForGraphPin(const FHeartGraphPinDesc& GraphPinDesc, UClass* VisualizerBase = nullptr) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry", meta = (DeterminesOutputType = "VisualizerBase"))
	virtual UClass* GetVisualizerClassForGraphConnection(const FHeartGraphPinDesc& FromPinDesc, const FHeartGraphPinDesc& ToPinDesc, UClass* VisualizerBase = nullptr) const;

	template <typename TBaseClass>
	TSubclassOf<TBaseClass> GetVisualizerClassForGraphNode(TSubclassOf<UHeartGraphNode> GraphNodeClass) const
	{
		return GetVisualizerClassForGraphNode(GraphNodeClass, TBaseClass::StaticClass());
	}

	template <typename TBaseClass>
	TSubclassOf<TBaseClass> GetVisualizerClassForGraphPin(const FHeartGraphPinDesc& GraphPinDesc) const
	{
		return GetVisualizerClassForGraphPin(GraphPinDesc, TBaseClass::StaticClass());
	}

	template <typename TBaseClass>
	TSubclassOf<TBaseClass> GetVisualizerClassForGraphConnection(const FHeartGraphPinDesc& FromPinDesc, const FHeartGraphPinDesc& ToPinDesc) const
	{
		return GetVisualizerClassForGraphConnection(FromPinDesc, ToPinDesc, TBaseClass::StaticClass());
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	void AddRegistrar(const UGraphNodeRegistrar* Registrar);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeRegistry")
	void RemoveRegistrar(const UGraphNodeRegistrar* Registrar);

	// Broadcast on any change to registered class lists.
	UPROPERTY(BlueprintAssignable, Category = "Heart|GraphNodeRegistry|Events")
	FHeartGraphNodeRegistryEvent OnRegistryChanged;

private:
	// Broadcast on any change to registered class lists.
	FHeartGraphNodeRegistryEventNative OnRegistryChangedNative;

	struct FNodeSourceEntry
	{
		TSet<Heart::Containers::TCountedWeakClassPtr<UHeartGraphNode>> GraphNodes;
		TArray<TObjectPtr<UClass>> RecursiveChildren;
		uint32 RecursiveRegistryCounter = 0;
	};

	TMap<FHeartNodeSource, FNodeSourceEntry> NodeRootTable;

	// Maps Graph Node classes to the visualizer class that can represent them in a displayed graph.
	TMap<TSubclassOf<UHeartGraphNode>, TSet<Heart::Containers::TCountedWeakPtr<UClass>>> NodeVisualizerMap;

	// Maps GraphPinTags to the visualizer class that can represent them in a displayed graph.
	TMap<FHeartGraphPinTag, TSet<Heart::Containers::TCountedWeakPtr<UClass>>> PinVisualizerMap;

	// Maps GraphPinTags to the visualizer class that can represent their connections in a displayed graph.
	TMap<FHeartGraphPinTag, TSet<Heart::Containers::TCountedWeakPtr<UClass>>> ConnectionVisualizerMap;

	// We have to store these hard-ref'd to keep around the stuff in GraphClasses as we cannot UPROP TMaps of TSets
	UPROPERTY()
	TArray<TObjectPtr<const UGraphNodeRegistrar>> ContainedRegistrars;
};