// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartNodeSource.h"
#include "HeartRegistrationClasses.h"
#include "General/CountedPtr.h"
#include "Model/HeartGraphPinTag.h"
#include "Model/HeartGraphNode.h"

#include "HeartGraphNodeRegistry.generated.h"

struct FHeartGraphPinDesc;
class UHeartGraphNode;
class UGraphNodeRegistrar;

class UHeartGraphNodeRegistry;
using FHeartGraphNodeRegistryEventNative = TMulticastDelegate<void(UHeartGraphNodeRegistry*)>;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeartGraphNodeRegistryEvent, UHeartGraphNodeRegistry*, Registry);

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FNodeSourceFilter, const FHeartNodeSource, NodeSource);
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FNodeSourceSort, const FHeartNodeSource, NodeSourceA, const FHeartNodeSource, tNodeSourceB);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(double, FNodeSourceScore, const FHeartNodeSource, tNodeSource);

USTRUCT(BlueprintType)
struct HEART_API FHeartRegistryQuery
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartRegistryQuery|Filter", NoClear)
	TSubclassOf<UHeartGraphNode> HeartGraphNodeBaseClass = UHeartGraphNode::StaticClass();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartRegistryQuery|Filter", NoClear)
	UClass* NodeObjectBaseClass = UObject::StaticClass();

	// Maximum Node Object classes to return. Set to 0 to allow unlimited results.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HeartRegistryQuery|Filter", meta = (ClampMin = 0))
	int32 MaxResults = 0;

	// Callback to filter the Node Objects.
	UPROPERTY(BlueprintReadWrite, Category = "HeartRegistryQuery|Filter")
	FNodeSourceFilter Filter;

	// Callback to sort a pair of Node Objects by Predicate, using Algo::Sort.
	UPROPERTY(BlueprintReadWrite, Category = "HeartRegistryQuery|Sort")
	FNodeSourceSort Sort;

	// Callback to sort Node Objects by providing each a score, and arranging them highest to lowest.
	// Will not be used if Sort is set.
	UPROPERTY(BlueprintReadWrite, Category = "HeartRegistryQuery|Sort")
	FNodeSourceScore Score;
};


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

protected:
	bool FilterObjectForRegistration(const UObject* Object) const;

	void AddRegistrationList(const FHeartRegistrationClasses& Registration, bool Broadcast = true);
	void RemoveRegistrationList(const FHeartRegistrationClasses& Registration, bool Broadcast = true);

	void BroadcastChange();

public:
	bool IsRegistered(const UGraphNodeRegistrar* Registrar) const;

	FHeartGraphNodeRegistryEventNative& GetOnRegistryChangedNative() { return OnRegistryChangedNative; }

	void ForEachNodeObjectClass(const TFunctionRef<bool(TSubclassOf<UHeartGraphNode>, FHeartNodeSource)>& Iter) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	TArray<FString> GetNodeCategories() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void GetAllNodeSources(TArray<FHeartNodeSource>& OutNodeSources) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void GetAllGraphNodeClassesAndNodeSources(TMap<FHeartNodeSource, TSubclassOf<UHeartGraphNode>>& OutClasses) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void QueryNodeClasses(const FHeartRegistryQuery& Query, TArray<FHeartNodeSource>& OutNodeSources) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphNodeRegistry")
	void QueryGraphAndNodeClasses(const FHeartRegistryQuery& Query, TMap<FHeartNodeSource, TSubclassOf<UHeartGraphNode>>& OutClasses) const;


	/**
	 * Get the graph node class that we use to represent the given arbitrary class.
	 */
	UE_DEPRECATED(5.4, TEXT("Please use GetGraphNodeClassesForNode or another method of determining a NodeSource's graph node"))
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

	// Maps Graph Node classes to the visualizer class that can represent them in an interactive graph.
	TMap<TSubclassOf<UHeartGraphNode>, TSet<Heart::Containers::TCountedWeakPtr<UClass>>> NodeVisualizerMap;

	// Maps Graph Pin classes to the visualizer class that can represent them in an interactive graph.
	TMap<FHeartGraphPinTag, TSet<Heart::Containers::TCountedWeakPtr<UClass>>> PinVisualizerMap;

	// We have to store these hard-ref'd to keep around the stuff in GraphClasses as we cannot UPROP TMaps of TMaps
	UPROPERTY()
	TArray<TObjectPtr<const UGraphNodeRegistrar>> ContainedRegistrars;
};