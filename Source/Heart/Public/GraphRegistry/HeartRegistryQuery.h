// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphNodeRegistry.h"
#include "HeartNodeSource.h"
#include "Model/HeartQueries.h"
#include "Model/HeartQueryObject.h"
#include "HeartRegistryQuery.generated.h"

class UHeartGraphSchema;
template <typename T> class TSubclassOf;

namespace Heart::Query
{
	// A single registered node/graph pair key
	struct FRegistryKey
	{
		int32 RootIndex = INDEX_NONE;
		int32 NodesIndex = INDEX_NONE; // Index into GraphNodes
		int32 RecursiveIndex = INDEX_NONE-1; // Index into RecursiveChildren

		friend bool operator==(const FRegistryKey& Lhs, const FRegistryKey& Rhs)
		{
			return Lhs.RootIndex == Rhs.RootIndex &&
				Lhs.NodesIndex == Rhs.NodesIndex &&
					Lhs.RecursiveIndex == Rhs.RecursiveIndex;
		}

		friend bool operator!=(const FRegistryKey& Lhs, const FRegistryKey& Rhs) { return !(Lhs == Rhs); }

		friend uint32 GetTypeHash(const FRegistryKey& Key)
		{
			return HashCombineFast(GetTypeHash(Key.RootIndex),
				HashCombineFast(GetTypeHash(Key.NodesIndex),
					GetTypeHash(Key.RecursiveIndex)));
		}
	};

	// A single registered node/graph pair value
	struct FRegistryValue
	{
		FHeartNodeSource Source;
		TObjectPtr<UClass> GraphNode;
	};

	// A single registered node/graph pair value
	struct FRegistryPair
	{
		FRegistryKey Key;
		FRegistryValue Value;
	};

	class HEART_API FRegistryQueryResult : public TMapQueryBase<FRegistryQueryResult, FRegistryKey, FRegistryValue>
	{
		friend TMapQueryBase;
		friend UHeartGraphNodeRegistry;

		class FIterator
		{
		public:
			explicit FIterator(const UHeartGraphNodeRegistry* Registry)
			  : Registry(Registry)
			{
				if (IsValid(Registry))
				{
					SourceMax = Registry->NodeRootTable.Num();
					Advance();
				}
			}

			explicit FIterator()
			  : Registry(nullptr),
				SourceMax(0) {}

			FIterator& operator++()
			{
				Advance();
				return *this;
			}

			FIterator operator++(int)
			{
				const FIterator Temp(*this);
				Advance();
				return Temp;
			}

			explicit operator bool() const
			{
				return IsValid(Registry);
			}

			FORCEINLINE bool operator==(const FIterator& Other) const
			{
				return Registry == Other.Registry &&
						Key == Other.Key;
			}
			FORCEINLINE bool operator!=(const FIterator& Other) const
			{
				return !(*this == Other);
			}

			FORCEINLINE FRegistryPair operator*() const { return {Key, Value}; }

		private:
			void Advance()
			{
				if (++Key.RecursiveIndex < RecursiveMax)
				{
					if (Key.RecursiveIndex > INDEX_NONE)
					{
						Value.Source = FHeartNodeSource(RootPair->Value.RecursiveChildren[Key.RecursiveIndex]);
						return;
					}
				}

				if (++Key.NodesIndex < GraphNodeMax)
				{
					RecursiveMax = RootPair->Value.RecursiveChildren.Num();
					Key.RecursiveIndex = INDEX_NONE;
					return UpdateNode();
				}

				if (++Key.RootIndex < SourceMax)
				{
					RootPair = &Registry->NodeRootTable.Get(FSetElementId::FromInteger(Key.RootIndex));

					GraphNodeMax = RootPair->Value.NodeClasses.Classes.Num();
					if (GraphNodeMax > 0)
					{
						Key.NodesIndex = 0;
						UpdateNode();
					}

					RecursiveMax = RootPair->Value.RecursiveChildren.Num();
					Key.RecursiveIndex = INDEX_NONE;
					Value.Source = RootPair->Key;
					return;
				}

				// All cases failed to increment, iteration is over.
				Registry = nullptr;
				Key = FRegistryKey();
			}

			void UpdateNode()
			{
				Value.GraphNode = RootPair->Value.NodeClasses.Classes[FSetElementId::FromInteger(Key.NodesIndex)].Obj;
			}

		public:
			FRegistryKey Key;
			FRegistryValue Value;

		private:
			const UHeartGraphNodeRegistry* Registry;
			int32 SourceMax;
			int32 GraphNodeMax = 0;
			int32 RecursiveMax = 0;
			const TPair<FHeartNodeSource, UHeartGraphNodeRegistry::FNodeSourceEntry>* RootPair = nullptr;
		};

		// A for-each range for iterating over the reference data
		struct FRange
		{
			explicit FRange(const UHeartGraphNodeRegistry* Registry)
			  : Iterator(FIterator(Registry)) {}

			FIterator Iterator;
		};

		FORCEINLINE friend FIterator begin(const FRange& Range) { return Range.Iterator; }
		FORCEINLINE friend FIterator end  (const FRange& Range) { return FIterator(); }

	public:
		FRegistryQueryResult(const UHeartGraphNodeRegistry* Registry);

		// Stop TMapQueryBase from trying to use operator<
		FRegistryQueryResult& CustomSort() { return *this; }

		int32 SrcNum() const { return CachedNum; }

		FRegistryValue operator[](const FRegistryKey Key) const;

		FIterator begin() const { return FIterator(Registry.Get()); }
		FIterator end  () const { return FIterator(); }

	private:
		const TWeakObjectPtr<const UHeartGraphNodeRegistry> Registry;
		const int32 CachedNum;
	};
}


DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FHeartRegistryBlueprintSort, FHeartNodeSource&, NodeSourceA, FHeartNodeSource&, NodeSourceB);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FHeartRegistryBlueprintFilter, const FHeartNodeSource, NodeSource);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(double, FHeartRegistryBlueprintScore, const FHeartNodeSource, NodeSource);

/**
 *
 */
UCLASS()
class HEART_API UHeartRegistryQuery : public UHeartQueryObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|RegistryQuery")
	void Run(const TSubclassOf<UHeartGraphSchema>& SchemaClass, TArray<FHeartNodeArchetype>& Results);

	UFUNCTION(BlueprintCallable, Category = "Heart|RegistryQuery")
	void AddFilter(const FHeartRegistryBlueprintFilter& Predicate);

	UFUNCTION(BlueprintCallable, Category = "Heart|RegistryQuery")
	void ClearFilters();

	UFUNCTION(BlueprintCallable, Category = "Heart|RegistryQuery")
	void SetSortByComparison(const FHeartRegistryBlueprintSort& Predicate);

	UFUNCTION(BlueprintCallable, Category = "Heart|RegistryQuery")
	void SetSortByScore(const FHeartRegistryBlueprintScore& Predicate);

	// Use the built-in sort function for the queried data.
	UFUNCTION(BlueprintCallable, Category = "Heart|RegistryQuery")
	void SetSortByDefault();

	UFUNCTION(BlueprintCallable, Category = "Heart|RegistryQuery")
	void ClearSort();

protected:
	TArray<FScriptDelegate> ScriptFilters;
	FScriptDelegate ScriptSort;

	enum ESortMode
	{
		// Perform no sorting
		Off,

		// Use default sort
		Default,

		// Boolean predicate sort by TLess<>
		Comparison,

		// Float predicate scored sort
		Score
	};
	ESortMode SortMode;
};