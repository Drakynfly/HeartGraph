// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraph.h"
#include "HeartGuids.h"
#include "HeartGraphPinDesc.h"
#include "HeartPinData.h"

struct FHeartNodePinData;
class UHeartGraphNode;
class UHeartGraph;

namespace Heart::Query
{
	class IMapQuery : public TSharedFromThis<IMapQuery> {}; // Stub for storing pointers to MapQueries

	template <typename QueryType, typename KeyType, typename ValueType>
	class TMapQueryBase : public IMapQuery
	{
		FORCEINLINE		  QueryType& AsType()		{ return *static_cast<		QueryType*>(this); }
		FORCEINLINE const QueryType& AsType() const { return *static_cast<const QueryType*>(this); }
		FORCEINLINE auto Lookup(KeyType Key) const { return AsType()[Key]; }

		FORCEINLINE int32 RefNum() const { return AsType().Num(); }

		template <typename Predicate, typename RetVal = void>
		struct TIsPredicate
		{
			static constexpr bool IsKeyPredicate = std::is_invocable_r_v<RetVal, Predicate, KeyType>;
			static constexpr bool IsValuePredicate = std::is_invocable_r_v<RetVal, Predicate, const ValueType&>;
			static constexpr bool IsKeyValuePredicate = std::is_invocable_r_v<RetVal, Predicate, KeyType, const ValueType&>;
			static constexpr bool IsValidPredicate = IsKeyPredicate || IsValuePredicate || IsKeyValuePredicate;

			enum
			{
				Value = IsValidPredicate
			};
		};

		// A for-each range for iterating over the reference data
		struct FQueryRange
		{
			explicit FQueryRange(const TMapQueryBase* Query)
			  : Query(Query) {}

			const TMapQueryBase* Query;
		};

		FORCEINLINE friend auto begin(const FQueryRange& Range) { return Range.Query->AsType().begin(); }
		FORCEINLINE friend auto end  (const FQueryRange& Range) { return Range.Query->AsType().end(); }

	public:
		/**
		 * Removes all results from the query that fail a predicate.
		 */
		template <
			typename Predicate
			UE_REQUIRES(TIsPredicate<Predicate, bool>::Value)
		>
		QueryType& Filter(Predicate Pred)
		{
			InitResults();

			for (auto It = Results.GetValue().CreateIterator(); It; ++It)
			{
				if constexpr (TIsPredicate<Predicate, bool>::IsKeyPredicate)
				{
					if (!Pred(*It))
					{
						It.RemoveCurrentSwap();
					}
				}

				if constexpr (TIsPredicate<Predicate, bool>::IsValuePredicate)
				{
					if (!Pred(Lookup(*It)))
					{
						It.RemoveCurrentSwap();
					}
				}

				if constexpr (TIsPredicate<Predicate, bool>::IsKeyValuePredicate)
				{
					if (!Pred(*It, Lookup(*It)))
					{
						It.RemoveCurrentSwap();
					}
				}
			}

			return AsType();
		}

		//using FFilter = TDelegate<bool(const ValueType&)>;
		using FFilter = TDelegate<bool(ValueType)>; // @todo stop copying the value

		/**
		 * Removes all results from the query that fail a delegate.
		 */
		template <typename UserClass, typename... VarTypes>
		QueryType& Filter_UObject(UserClass* InUserObject,
			typename FFilter::template TMethodPtr<UserClass, VarTypes...> InFunc, VarTypes... Vars)
		{
			FFilter Delegate = FFilter::CreateUObject(InUserObject, InFunc, Forward<VarTypes>(Vars)...);

			InitResults();

			for (auto It = Results.GetValue().CreateIterator(); It; ++It)
			{
				if (!Delegate.Execute(Lookup(*It)))
				{
					It.RemoveCurrentSwap();
				}
			}

			return AsType();
		}

		/**
		 * Removes all results from the query that fail a delegate.
		 */
		template <typename UserClass, typename... VarTypes>
		QueryType& Filter_UObject(UserClass* InUserObject,
			typename FFilter::template TConstMethodPtr<UserClass, VarTypes...> InFunc, VarTypes... Vars)
		{
			FFilter Delegate = FFilter::CreateUObject(InUserObject, InFunc, Forward<VarTypes>(Vars)...);

			InitResults();

			for (auto It = Results.GetValue().CreateIterator(); It; ++It)
			{
				if (!Delegate.Execute(Lookup(*It)))
				{
					It.RemoveCurrentSwap();
				}
			}

			return AsType();
		}

		/**
		 * Iterate over all results currently in the query.
		 */
		template <
			typename Predicate
			UE_REQUIRES(TIsPredicate<Predicate>::Value)
		>
		QueryType& ForEach(Predicate Pred)
		{
			if (Results.IsSet())
			{
				for (auto&& Key : Results.GetValue())
				{
					if constexpr (TIsPredicate<Predicate>::IsKeyPredicate)
					{
						Pred(Key);
					}

					if constexpr (TIsPredicate<Predicate>::IsValuePredicate)
					{
						Pred(Lookup(Key));
					}

					if constexpr (TIsPredicate<Predicate>::IsKeyValuePredicate)
					{
						Pred(Key, Lookup(Key));
					}
				}
			}
			else
			{
				for (auto&& Element : FQueryRange(this))
				{
					if constexpr (TIsPredicate<Predicate>::IsKeyPredicate)
					{
						Pred(Element.Key);
					}

					if constexpr (TIsPredicate<Predicate>::IsValuePredicate)
					{
						Pred(Element.Value);
					}

					if constexpr (TIsPredicate<Predicate>::IsKeyValuePredicate)
					{
						Pred(Element.Key, Element.Value);
					}
				}
			}

			return AsType();
		}

		//using FCallback = TDelegate<void(const ValueType&)>;
		using FCallback = TDelegate<void(ValueType)>; // @todo stop copying the value

		/**
		 * Iterate over all results currently in the query.
		 */
		template <typename UserClass, typename... VarTypes>
		QueryType& ForEach_UObject(UserClass* InUserObject,
			typename FCallback::template TMethodPtr<UserClass, VarTypes...> InFunc, VarTypes... Vars)
		{
			FCallback Delegate = FCallback::CreateUObject(InUserObject, InFunc, Forward<VarTypes>(Vars)...);

			if (Results.IsSet())
			{
				for (auto&& Key : Results.GetValue())
				{
					Delegate.Execute(Lookup(Key));
				}
			}
			else
			{
				for (auto&& Element : FQueryRange(this))
				{
					Delegate.Execute(Element.Value);
				}
			}

			return AsType();
		}

		/**
		 * Iterate over all results currently in the query.
		 */
		template <typename UserClass, typename... VarTypes>
		QueryType& ForEach_UObject(const UserClass* InUserObject,
			typename FCallback::template TConstMethodPtr<UserClass, VarTypes...> InFunc, VarTypes... Vars)
		{
			FCallback Delegate = FCallback::CreateUObject(InUserObject, InFunc, Forward<VarTypes>(Vars)...);

			if (Results.IsSet())
			{
				for (auto&& Key : Results.GetValue())
				{
					Delegate.Execute(Lookup(Key));
				}
			}
			else
			{
				for (auto&& Element : FQueryRange(this))
				{
					Delegate.Execute(Element.Value);
				}
			}

			return AsType();
		}

		template <typename Predicate>
		TOptional<KeyType> Find(Predicate Pred) const
		{
			if (Results.IsSet())
			{
				for (auto&& Key : Results.GetValue())
				{
					if (auto Result = Pred(Lookup(Key));
						Result.IsSet())
					{
						return Result.GetValue();
					}
				}
			}
			else
			{
				for (auto&& Element : FQueryRange(this))
				{
					if (auto Result = Pred(Element.Value);
						Result.IsSet())
					{
						return Result.GetValue();
					}
				}
			}

			return {};
		}

		GENERATE_MEMBER_FUNCTION_CHECK(DefaultSort, QueryType&, , );

		// Sort the results by their default order
		QueryType& Sort()
		{
			if constexpr (THasMemberFunction_DefaultSort<QueryType>::Value)
			{
				AsType().DefaultSort();
			}
			else
			{
				InitResults();
				Algo::Sort(Results.GetValue());
			}

			return AsType();
		}

		template <typename Predicate>
		QueryType& Sort(Predicate Pred)
		{
			InitResults();
			Algo::Sort(Results.GetValue(), Pred);
			return AsType();
		}

		template <typename ProjectionType>
		QueryType& SortBy(ProjectionType Proj)
		{
			InitResults();
			Algo::SortBy(Results.GetValue(), Proj);
			return AsType();
		}

		template <typename ProjectionType, typename Predicate>
		QueryType& SortBy(ProjectionType Proj, Predicate Pred)
		{
			InitResults();
            Algo::SortBy(Results.GetValue(), Proj, Pred);
            return AsType();
		}

		// Inline sort by predicate if boolean is true
		QueryType& SortIf(const bool bShouldSort)
		{
			return bShouldSort ? Sort() : AsType();
		}

		// Inline sort by predicate if boolean is true
		template <typename... Args>
		QueryType& SortByIf(const bool bShouldSort, Args... InArgs)
		{
			return bShouldSort ? SortBy(Forward<Args>(InArgs)...) : AsType();
		}

		QueryType& Invert()
		{
			// If Results is not initialized, or equal to the reference data, set to an empty array.
			if (!Results.IsSet() || Results->Num() == RefNum())
			{
				Results = TArray<KeyType>();
				return AsType();
			}

			// Create array from source data
			TArray<KeyType> NewResults;
			NewResults.SetNum(RefNum());
			for (auto&& Element : FQueryRange(this))
			{
				NewResults.Emplace(Element.Key);
			}

			// Remove all elements currently in the result
			ForEach([&NewResults](const KeyType Key)
				{
					NewResults.Remove(Key);
				});

			NewResults.Shrink();

			// Apply
			Results = NewResults;

			return AsType();
		}

		const TArray<KeyType>& Get()
		{
			InitResults();
			return Results.GetValue();
		}

		int32 Num() const
		{
			// If results has been initialized return that num
			if (Results.IsSet())
			{
				return Results->Num();
			}

			// Otherwise, return the source data num
			return RefNum();
		}

	private:
		// This function makes a copy of the reference data, and stores it in Results, where is can be pruned down by
		// filters, or re-ordered by sorting.
		void InitResults()
		{
			if (!Results.IsSet())
			{
				Results = TArray<KeyType>();
				Results->Reserve(RefNum());
				for (auto&& Element : FQueryRange(this))
				{
					Results->Emplace(Element.Key);
				}
				checkSlow(Results->Num() = RefNum())
			}
		}

		TOptional<TArray<KeyType>> Results;
	};

	class HEART_API FPinQueryResult : public TMapQueryBase<FPinQueryResult, FHeartPinGuid, FHeartGraphPinDesc>
	{
		friend TMapQueryBase;

		using FMapType = TMap<FHeartPinGuid, FHeartGraphPinDesc>;

	public:
		FPinQueryResult(const FHeartNodePinData& Src);

		// Sort the results by their Pin Order
		FPinQueryResult& DefaultSort();

		int32 Num() const {	return Reference.PinDescriptions.Num(); }

		FORCEINLINE auto& operator[](const FHeartPinGuid PinGuid) const
		{
			return Reference.PinDescriptions[PinGuid];
		}

		FORCEINLINE auto begin() const { return Reference.PinDescriptions.begin(); }
		FORCEINLINE auto end() const { return Reference.PinDescriptions.end(); }

	private:
		const FHeartNodePinData& Reference;
	};

	using FNodeMap = TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>>;

	template <typename Impl>
	class TNodeQueryResultBase : public TMapQueryBase<TNodeQueryResultBase<Impl>, FHeartNodeGuid, UHeartGraphNode*>
	{
		friend TMapQueryBase;

		FORCEINLINE const FNodeMap& Data() const { return (*static_cast<const Impl*>(this)).Internal_Data(); }

	public:
		int32 Num() const {	return Data().Num(); }

		FORCEINLINE UHeartGraphNode* operator[](const FHeartNodeGuid NodeGuid) const
		{
			return Data()[NodeGuid];
		}

		FORCEINLINE auto begin() const { return Data().begin(); }
		FORCEINLINE auto end  () const { return Data().end(); }
	};

	template <typename Source>
	class TNodeQueryResult
	{
	};

	template<>
	class HEART_API TNodeQueryResult<FNodeMap> : public TNodeQueryResultBase<TNodeQueryResult<FNodeMap>>
	{
		friend TNodeQueryResultBase;

	public:
		// Initialize a query for an array of loose nodes
		TNodeQueryResult(const TConstArrayView<TObjectPtr<UHeartGraphNode>>& Src);

	private:
		auto& Internal_Data() const { return Reference; }

		const FNodeMap Reference;
	};

	template<>
	class HEART_API TNodeQueryResult<UHeartGraph*> : public TNodeQueryResultBase<TNodeQueryResult<UHeartGraph*>>
	{
		friend TNodeQueryResultBase;

	public:
		// Initialize a query for all nodes in a graph
		TNodeQueryResult(const UHeartGraph* Src);

	private:
		auto& Internal_Data() const { return Reference->GetNodes(); }

		const UHeartGraph* Reference;
	};
}