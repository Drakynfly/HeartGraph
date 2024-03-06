// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphNetProxy.h"
#include "HeartGuids.h"

struct FHeartGraphPinDesc;
struct FHeartNodePinData;
class UHeartGraphNode;
class UHeartGraph;

namespace Heart::Query
{
	template <typename QueryType, typename DataType, typename ValueType>
	class TMapQueryBase
	{
		FORCEINLINE		  QueryType& AsType()		{ return *static_cast<		QueryType*>(this); }
		FORCEINLINE const QueryType& AsType() const { return *static_cast<const QueryType*>(this); }

	public:
		/**
		 * Removes all results from the query that fail a predicate.
		 */
		template <typename Predicate>
		typename TEnableIf<std::is_invocable_r_v<bool, Predicate, const ValueType&>, QueryType&>::Type Filter(Predicate Pred)
		{
			AsType().InitResults();

			for (auto It = Results.GetValue().CreateIterator(); It; ++It)
			{
				if (!Pred(AsType().Internal_GetMap()[*It]))
				{
					It.RemoveCurrentSwap();
				}
			}

			return AsType();
		}

		/**
		 * Removes all results from the query that fail a predicate.
		 */
		template <typename Predicate>
		typename TEnableIf<std::is_invocable_r_v<bool, Predicate, DataType, const ValueType&>, QueryType&>::Type Filter(Predicate Pred)
		{
			AsType().InitResults();

			for (auto It = Results.GetValue().CreateIterator(); It; ++It)
			{
				if (!Pred(*It, AsType().Internal_GetMap()[*It]))
				{
					It.RemoveCurrentSwap();
				}
			}

			return AsType();
		}

		using FFilter = TDelegate<bool(ValueType)>;

		/**
		 * Removes all results from the query that fail a delegate.
		 */
		template <typename UserClass, typename... VarTypes>
		QueryType& Filter_UObject(UserClass* InUserObject,
			typename FFilter::template TMethodPtr<UserClass, VarTypes...> InFunc, VarTypes... Vars)
		{
			FFilter Delegate = FFilter::CreateUObject(InUserObject, InFunc, Forward<VarTypes>(Vars)...);

			AsType().InitResults();

			for (auto It = Results.GetValue().CreateIterator(); It; ++It)
			{
				if (!Delegate.Execute(AsType().Internal_GetMap()[*It]))
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

			AsType().InitResults();

			for (auto It = Results.GetValue().CreateIterator(); It; ++It)
			{
				if (!Delegate.Execute(AsType().Internal_GetMap()[*It]))
				{
					It.RemoveCurrentSwap();
				}
			}

			return AsType();
		}

		/**
		 * Iterate over all results currently in the query.
		 */
		template <typename Predicate>
		QueryType& ForEach(Predicate Pred)
		{
			if (Results.IsSet())
			{
				for (auto&& Key : Results.GetValue())
				{
					Pred(AsType().Internal_GetMap()[Key]);
				}
			}
			else
			{
				for (auto&& Key : AsType().Internal_GetMap())
				{
					Pred(Key.Value);
				}
			}

			return AsType();
		}

		using FCallback = TDelegate<void(ValueType)>;

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
					Delegate.Execute(AsType().Internal_GetMap()[Key]);
				}
			}
			else
			{
				for (auto&& Key : AsType().Internal_GetMap())
				{
					Delegate.Execute(Key.Value);
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
					Delegate.Execute(AsType().Internal_GetMap()[Key]);
				}
			}
			else
			{
				for (auto&& Key : AsType().Internal_GetMap())
				{
					Delegate.Execute(Key.Value);
				}
			}

			return AsType();
		}

		template <typename Predicate>
		TOptional<DataType> Find(Predicate Pred) const
		{
			if (Results.IsSet())
			{
				for (auto&& Key : Results.GetValue())
				{
					if (auto Result = Pred(AsType().Internal_GetMap()[Key]);
						Result.IsSet())
					{
						return Result.GetValue();
					}
				}
			}
			else
			{
				for (auto&& Key : AsType().Internal_GetMap())
				{
					if (auto Result = Pred(Key.Value);
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

			return AsType();
		}

		// Sort the results by a predicate
		template <typename Predicate>
		QueryType& SortBy(Predicate Pred)
		{
			AsType().InitResults();
			Algo::Sort(Results.GetValue(), Pred);
			return AsType();
		}

		// Inline sort if boolean is true
		QueryType& Sort(const bool bShouldSort)
		{
			return bShouldSort ? Sort() : AsType();
		}

		// Inline sort by predicate if boolean is true
		template <typename Predicate>
		QueryType& Sort(const bool bShouldSort, Predicate Pred)
		{
			return bShouldSort ? SortBy(Pred) : AsType();
		}

		TArray<DataType> Get() const
		{
			if (Results.IsSet())
			{
				return Results.GetValue();
			}

			TArray<DataType> Out;
			AsType().Internal_GetOptions(Out);
			return Out;
		}

	protected:
		void InitResults()
		{
			if (!Results.IsSet())
			{
				TArray<DataType> Temp;
				AsType().Internal_GetOptions(Temp);
				Results = MoveTemp(Temp);
			}
		}

		TOptional<TArray<DataType>> Results;
	};

	class HEART_API FPinQueryResult : public TMapQueryBase<FPinQueryResult, FHeartPinGuid, FHeartGraphPinDesc>
	{
		friend TMapQueryBase;

	public:
		FPinQueryResult(const FHeartNodePinData& Src);

		// Sort the results by their Pin Order
		FPinQueryResult& DefaultSort();

	private:
		void Internal_GetOptions(TArray<FHeartPinGuid>& Options) const;
		const TMap<FHeartPinGuid, FHeartGraphPinDesc>& Internal_GetMap() const;

		const FHeartNodePinData& Reference;
	};

	class HEART_API FNodeQueryResult : public TMapQueryBase<FNodeQueryResult, FHeartNodeGuid, UHeartGraphNode*>
	{
		friend TMapQueryBase;

	public:
		// Initialize a query for all nodes in a graph
		FNodeQueryResult(const UHeartGraph* Src);

		// Initialize a query for an array of loose nodes
		FNodeQueryResult(const TConstArrayView<TObjectPtr<UHeartGraphNode>>& Src);

	private:
		void Internal_GetOptions(TArray<FHeartNodeGuid>& Options) const;
		const TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>>& Internal_GetMap() const;

		using AsGraph = const UHeartGraph*;
		using AsLoose = TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>>;

		const TVariant<AsGraph, AsLoose> Reference;
	};
}