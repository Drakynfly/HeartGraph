// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartNodeSource.h"
#include "General/CountedPtr.h"
#include "Model/HeartGraphPinTag.h"

#include "HeartGraphNodeRegistry.generated.h"

struct FHeartRegistryClassLists;

namespace Heart::Query
{
	class FRegistryQueryResult;
}

namespace Heart::Registry
{
	class FCounter
	{
	public:
		bool IsEmpty() const { return Classes.IsEmpty(); }

		// Increment the reference count for a class
		void Inc(UClass* Class)
		{
			Classes.FindOrAdd(Class).Inc();
		}

		// Decrement the reference count for a class
		void Dec(UClass* Class)
		{
			if (auto&& ObjPtr = Classes.Find(Class))
			{
				if (ObjPtr->Dec() <= 0)
				{
					Classes.Remove(*ObjPtr);
				}
			}
		}

		void CollectReferences(FReferenceCollector& Collector)
		{
			for (auto&& Value : Classes)
			{
				Collector.AddStableReference(&Value.Obj);
			}
		}

		TSet<Containers::TCountedPtr<UClass>> Classes;
	};

	template <typename TKey>
	class TTracker
	{
	public:
		bool IsEmpty() const { return Counters.IsEmpty(); }

		// Find the first valid class for a key
		TObjectPtr<UClass> Find(const TKey& Key) const
		{
			if (auto&& Counter = Counters.Find(Key))
			{
				for (auto&& CountedClass : Counter->Classes)
				{
					if (!IsValid(CountedClass.Obj))
					{
						continue;
					}

					return CountedClass.Obj;
				}
			}
			return nullptr;
		}

		// Find the first valid class for a key that is equal to or a child of a parent
		TObjectPtr<UClass> FindByClass(const TKey& Key, UClass* Parent) const
		{
			if (!IsValid(Parent)) return nullptr;

			if (auto&& Counter = Counters.Find(Key))
			{
				for (auto&& CountedClass : Counter->Classes)
				{
					if (!IsValid(CountedClass.Obj))
					{
						continue;
					}

					if (CountedClass.Obj->IsChildOf(Parent))
					{
						return CountedClass.Obj;
					}
				}
			}
			return nullptr;
		}

		// Increment the reference count for a class
		void Inc(const TKey& Key, UClass* Class)
		{
			if (!ensure(IsValid(Class))) return;
			Counters.FindOrAdd(Key).Inc(Class);
		}

		// Decrement the reference count for a class
		void Dec(const TKey& Key, UClass* Class)
		{
			if (!IsValid(Class)) return;

			if (auto&& Counter = Counters.Find(Key))
			{
				Counter->Dec(Class);
				if (Counter->IsEmpty())
				{
					Counters.Remove(Key);
				}
			}
		}

		void CollectReferences(FReferenceCollector& Collector)
		{
			for (auto&& Element : Counters)
			{
				Element.Value.CollectReferences(Collector);
			}
		}

	private:
		TMap<TKey, FCounter> Counters;
	};
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
 * classes, and they can be deregistered without removing classes that other registrars have also added.
 */
UCLASS()
class HEART_API UHeartGraphNodeRegistry : public UObject
{
	GENERATED_BODY()

	// Registrar is allowed to call AddRegistrationList/RemoveRegistrationList
	friend class UGraphNodeRegistrar;
	friend Heart::Query::FRegistryQueryResult;

public:
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

protected:
	bool FilterObjectForRegistration(const UObject* Object) const;

	void AddRegistrationList(const FHeartRegistryClassLists& List, bool Broadcast = true);
	void RemoveRegistrationList(const FHeartRegistryClassLists& List, bool Broadcast = true);

	void BroadcastChange();

public:
	FHeartGraphNodeRegistryEventNative::RegistrationType& GetOnRegistryChangedNative() { return OnRegistryChangedNative; }

	static void GatherReferences(const FHeartRegistryClassLists& List, TArray<FSoftObjectPath>& Objects);

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
	UFUNCTION(BlueprintCallable, meta = (DeprecatedFunction, DeprecationMessage = "Use GetGraphNodeClassesForNode"))
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
		Heart::Registry::FCounter NodeClasses;
		TArray<TObjectPtr<UClass>> RecursiveChildren;
		uint32 RecursiveRegistryCounter = 0;
	};

	// Maps a Node Source (a possible spawnable node type) to a counted set of Graph Node classes that supports the node type,
	// alongside recursively added child classes (if requested by a registrar)
	TMap<FHeartNodeSource, FNodeSourceEntry> NodeRootTable;

	// Maps Graph Node classes to the visualizer class that can represent them in a displayed graph.
	Heart::Registry::TTracker<TSubclassOf<UHeartGraphNode>> NodeVisualizers;

	// Maps GraphPinTags to the visualizer classes that can represent them in a displayed graph.
	Heart::Registry::TTracker<FHeartGraphPinTag> PinVisualizers;

	// Maps GraphPinTags to the visualizer classes that can represent their connections in a displayed graph.
	Heart::Registry::TTracker<FHeartGraphPinTag> ConnectionVisualizers;

	// Track Registrars to prevent multiple adds.
	UPROPERTY()
	TArray<TObjectPtr<const UGraphNodeRegistrar>> ContainedRegistrars;
};