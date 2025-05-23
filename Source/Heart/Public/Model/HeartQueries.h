﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Templates/UnrealTypeTraits.h"

namespace Heart::Query
{
	//class IMapQuery : public TSharedFromThis<IMapQuery> {}; // Stub for storing pointers to MapQueries

	// Static query behavior flags
	enum class EClassFlags
	{
		NoFlags = 0,
		PassKeyByRef = 1 << 0,
		PassValueByRef = 1 << 1,
	};
	ENUM_CLASS_FLAGS(EClassFlags)

	// Runtime query behavior flags
	enum EFlags
	{
		NoFlags = 0,

		// This is a potential optimization when calling Sort/SortBy with a potentially expensive callback.
		// Instead of making redundant calls, the projection results will be cached in a map, so each key is only ran
		// through the callback once.
		ProjectionCache = 1 << 0,
	};
	ENUM_CLASS_FLAGS(EFlags)

	enum EInvert { No, Invert };

	/**
	 * Map Queries are classes that can filter/sort/iterate keys of a TMap or map-like data structure.
	 * They *never* mutate the underlying data.
	 */
	template <typename QueryType, typename KeyType, typename ValueType, EClassFlags ClassFlags = EClassFlags::NoFlags>
	class TMapQueryBase /*: public IMapQuery*/
	{
	public:
		// Begin of templates/defines

		// Check if the implementing type has certain functions implemented that enable optional behavior
		struct FImplFeatures
		{
			using FSimpleData = TMap<KeyType, ValueType>;

			// A function that returns a TMap of the data to query. This is usually all that is required to implement for simple query types
			GENERATE_MEMBER_FUNCTION_CHECK(SimpleData, const FSimpleData&, const, );

			// Replaces the fallback Algo::Sort function with a custom one.
			GENERATE_MEMBER_FUNCTION_CHECK(CustomSort, QueryType&,, );

			static constexpr bool PassKeyByRef = EnumHasAnyFlags(ClassFlags, EClassFlags::PassKeyByRef);
			static constexpr bool PassValueByRef = EnumHasAnyFlags(ClassFlags, EClassFlags::PassValueByRef);
		};

		template<typename T, int32 Size>
		struct TLargerThan
		{
			enum
			{
				Value = sizeof(T) > Size
			};
		};

		// If the Key or Value type is larger than 8 bytes, pass-by-ref, otherwise, pass-by-value
		using PassedKey = std::conditional_t<TLargerThan<KeyType, 8>::Value || FImplFeatures::PassKeyByRef, const KeyType&, KeyType>;
		using PassedValue = std::conditional_t<TLargerThan<ValueType, 8>::Value || FImplFeatures::PassValueByRef, const ValueType&, ValueType>;
		using FStorage = TArray<KeyType>;

		template <typename Predicate, typename RetVal = void>
		struct TIsPredicate
		{
			static constexpr bool IsKeyPredicate = std::is_invocable_r_v<RetVal, Predicate, PassedKey>;
			static constexpr bool IsValuePredicate = std::is_invocable_r_v<RetVal, Predicate, PassedValue>;
			static constexpr bool IsKeyKeyPredicate = std::is_invocable_r_v<RetVal, Predicate, PassedKey, PassedKey>;
			static constexpr bool IsKeyValuePredicate = std::is_invocable_r_v<RetVal, Predicate, PassedKey, PassedValue>;
			static constexpr bool IsIterationPredicate = IsKeyPredicate || IsValuePredicate || IsKeyValuePredicate;
		};

		// End of templates/defines

	private:
		FORCEINLINE		  QueryType& AsType()		{ return *static_cast<		QueryType*>(this); }
		FORCEINLINE const QueryType& AsType() const { return *static_cast<const QueryType*>(this); }

		FORCEINLINE auto Lookup(KeyType Key) const
		{
			if constexpr (FImplFeatures::template THasMemberFunction_SimpleData<QueryType>::Value)
			{
				return AsType().SimpleData()[Key];
			}
			else
			{
				return AsType()[Key];
			}
		}

		FORCEINLINE int32 RefNum() const
		{
			if constexpr (FImplFeatures::template THasMemberFunction_SimpleData<QueryType>::Value)
			{
				return AsType().SimpleData().Num();
			}
			else
			{
				return AsType().SrcNum();
			}
		}

		// A for-each range for iterating over the reference data
		struct FQueryRange
		{
			explicit FQueryRange(const TMapQueryBase* Query)
			  : Query(Query) {}

			const TMapQueryBase* Query;
		};

		FORCEINLINE friend auto begin(const FQueryRange& Range)
		{
			if constexpr (FImplFeatures::template THasMemberFunction_SimpleData<QueryType>::Value)
			{
				return Range.Query->AsType().SimpleData().begin();
			}
			else
			{
				return Range.Query->AsType().begin();
			}
		}

		FORCEINLINE friend auto end(const FQueryRange& Range)
		{
			if constexpr (FImplFeatures::template THasMemberFunction_SimpleData<QueryType>::Value)
			{
				return Range.Query->AsType().SimpleData().end();
			}
			else
			{
				return Range.Query->AsType().end();
			}
		}

		// Function to flip logic when Invert is enabled at compile time
		template <EInvert Invert>
		static constexpr bool Eval(const bool Value)
		{
			if constexpr (Invert == EInvert::Invert)
			{
				return !Value;
			}
			else
			{
				return Value;
			}
		};

	public:
		// Enable query features
		QueryType& Enable(const EFlags InFlags)
		{
			EnumAddFlags(Flags, InFlags);
			return AsType();
		}

		// Enable query features
		QueryType& Disable(const EFlags InFlags)
		{
			EnumRemoveFlags(Flags, InFlags);
			return AsType();
		}

		/**
		 * Removes all results from the query that fail a predicate.
		 */
		template <
			EInvert Invert = No,
			typename Predicate
			UE_REQUIRES(TIsPredicate<Predicate, bool>::IsIterationPredicate)
		>
		QueryType& Filter(Predicate Pred)
		{
			InitResults();

			for (auto It = Results.GetValue().CreateIterator(); It; ++It)
			{
				if constexpr (TIsPredicate<Predicate, bool>::IsKeyPredicate)
				{
					if (!Eval<Invert>(Pred(*It)))
					{
						It.RemoveCurrentSwap();
					}
				}

				if constexpr (TIsPredicate<Predicate, bool>::IsValuePredicate)
				{
					if (!Eval<Invert>(Pred(Lookup(*It))))
					{
						It.RemoveCurrentSwap();
					}
				}

				if constexpr (TIsPredicate<Predicate, bool>::IsKeyValuePredicate)
				{
					if (!Eval<Invert>(Pred(*It, Lookup(*It))))
					{
						It.RemoveCurrentSwap();
					}
				}
			}

			return AsType();
		}

		using FFilter = TDelegate<bool(PassedValue)>;

		/**
		 * Removes all results from the query that fail a delegate.
		 */
		template <
			EInvert Invert = No,
			typename UserClass,
			typename... VarTypes>
		QueryType& Filter_UObject(UserClass* InUserObject,
			typename FFilter::template TMethodPtr<UserClass, VarTypes...> InFunc, VarTypes... Vars)
		{
			FFilter Delegate = FFilter::CreateUObject(InUserObject, InFunc, Forward<VarTypes>(Vars)...);

			InitResults();

			for (auto It = Results.GetValue().CreateIterator(); It; ++It)
			{
				if (!Eval<Invert>(Delegate.Execute(Lookup(*It))))
				{
					It.RemoveCurrentSwap();
				}
			}

			return AsType();
		}

		/**
		 * Removes all results from the query that fail a delegate.
		 */
		template <
			EInvert Invert = No,
			typename UserClass,
			typename... VarTypes>
		QueryType& Filter_UObject(UserClass* InUserObject,
			typename FFilter::template TConstMethodPtr<UserClass, VarTypes...> InFunc, VarTypes... Vars)
		{
			FFilter Delegate = FFilter::CreateUObject(InUserObject, InFunc, Forward<VarTypes>(Vars)...);

			InitResults();

			for (auto It = Results.GetValue().CreateIterator(); It; ++It)
			{
				if (!Eval<Invert>(Delegate.Execute(Lookup(*It))))
				{
					It.RemoveCurrentSwap();
				}
			}

			return AsType();
		}

		/**
		 * Inverts the current data filtered.
		 * Usually, it would be preferred to pass Query::Invert through the filter calls directly, as a template parameter,
		 * but if choosing to invert at runtime, this is the alternative.
		 */
		QueryType& Invert(const EInvert InInvert = EInvert::Invert)
		{
			if (InInvert == EInvert::Invert)
			{
				// Results not being set is implicitly equal to the entire dataset, so the inversion is empty.
				// Likewise, if the Num in both arrays match, then they contain the same data as well.
				if (!Results.IsSet() || Results->Num() == RefNum())
				{
					Results = FStorage();
					return AsType();
				}

				// If Results is set, but empty, the inversion is all the data, so clear it to the implicit dataset.
				if (Results->Num() == 0)
				{
					Results.Reset();
					return AsType();
				}

				// Otherwise, manually flip the values.

				// Create a new array from source data...
				FStorage NewResults;
				NewResults.Reserve(RefNum());
				for (auto&& Element : FQueryRange(this))
				{
					NewResults.Emplace(Element.Key);
				}

				// ... remove all elements currently in the result...
				ForEach([&NewResults](const PassedKey Key)
					{
						NewResults.RemoveSingleSwap(Key);
					});
				NewResults.Shrink();

				// ...and assign.
				Results = NewResults;
			}

			return AsType();
		}

		/**
		 * Iterate over all results currently in the query.
		 */
		template <
			typename Predicate
			UE_REQUIRES(TIsPredicate<Predicate>::IsIterationPredicate)
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

		using FCallback = TDelegate<void(PassedValue)>;

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
					if (Pred(Lookup(Key)))
					{
						return Key;
					}
				}
			}
			else
			{
				for (auto&& Element : FQueryRange(this))
				{
					if (Pred(Element.Value))
					{
						return Element.Key;
					}
				}
			}

			return NullOpt;
		}

		// Sort the results by their default order
		QueryType& Sort()
		{
			if constexpr (FImplFeatures::template THasMemberFunction_CustomSort<QueryType>::Value)
			{
				AsType().CustomSort();
			}
			else
			{
				InitResults();
				Algo::Sort(Results.GetValue());
			}

			return AsType();
		}

		template <
			typename Predicate
			UE_REQUIRES(TIsPredicate<Predicate, bool>::IsKeyKeyPredicate)
		>
		QueryType& Sort(Predicate Pred)
		{
			InitResults();
			Algo::Sort(Results.GetValue(), Pred);
			return AsType();
		}

		template <typename ProjectionType>
		QueryType& SortBy(ProjectionType Proj)
		{
			return SortBy(Proj, TLess<>());
		}

		template <
			typename ProjectionType,
			typename Predicate
			UE_REQUIRES(TIsPredicate<ProjectionType>::IsKeyPredicate)
		>
		QueryType& SortBy(ProjectionType Proj, Predicate Pred)
		{
			using RetType = std::invoke_result_t<ProjectionType, PassedKey>;

			InitResults();

			if (EnumHasAnyFlags(Flags, ProjectionCache))
			{
				TMap<KeyType, RetType> Scores;

				auto CachingProjection =
					[&](const PassedKey Key)
					{
						if (const RetType* Score = Scores.Find(Key))
						{
							return *Score;
						}

						RetType RetVal = Proj(Key);
						Scores.Add(Key, RetVal);
						return RetVal;
					};

				Algo::SortBy(Results.GetValue(), CachingProjection, Pred);
			}
			else
			{
				Algo::SortBy(Results.GetValue(), Proj, Pred);
			}

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

		const FStorage& Get()
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
		// This function makes a copy of the reference data, and stores it in Results, where it can be pruned down by
		// filters, or re-ordered by sorting.
		void InitResults()
		{
			if (!Results.IsSet())
			{
				Results = FStorage();
				Results->Reserve(RefNum());
				for (auto&& Element : FQueryRange(this))
				{
					Results->Emplace(Element.Key);
				}
				checkSlow(Results->Num() == RefNum())
			}
		}

		TOptional<FStorage> Results;
		EFlags Flags;
	};
}