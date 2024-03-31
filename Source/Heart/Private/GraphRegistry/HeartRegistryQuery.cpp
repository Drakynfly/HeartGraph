// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphRegistry/HeartRegistryQuery.h"
#include "GraphRegistry/HeartGraphNodeRegistry.h"
#include "GraphRegistry/HeartRegistryRuntimeSubsystem.h"
#include "Algo/AllOf.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartRegistryQuery)

namespace Heart::Query
{
	FRegistryQueryResult::FRegistryQueryResult(const UHeartGraphNodeRegistry* Registry)
	  : Registry(Registry),
		CachedNum(Registry->GetNumNodes(true))
	{
	}

	FRegistryValue FRegistryQueryResult::operator[](const FRegistryKey Key) const
	{
		auto&& Entry = Registry->NodeRootTable.Get(FSetElementId::FromInteger(Key.RootIndex));

		return FRegistryValue{
			Key.RecursiveIndex != INDEX_NONE ? FHeartNodeSource(Entry.Value.RecursiveChildren[Key.RecursiveIndex]) : Entry.Key,
			Entry.Value.GraphNodes[FSetElementId::FromInteger(Key.NodesIndex)].Obj.Get()};
	}
}

void UHeartRegistryQuery::Run(const TSubclassOf<UHeartGraph>& GraphClass, TArray<FHeartNodeArchetype>& Results)
{
	auto&& Subsystem = GEngine->GetEngineSubsystem<UHeartRegistryRuntimeSubsystem>();
	if (!IsValid(Subsystem))
	{
		return;
	}

	Heart::Query::FRegistryQueryResult Query(Subsystem->GetRegistry(GraphClass));

	// Enabling the ProjectionCache is a good idea here because we are jumping into the BP VM, which is
	// undoubtably less efficient then hashing the results
	Query.Enable(Heart::Query::ProjectionCache);

	Query.Filter(
	[&](const Heart::Query::FRegistryValue& Value)
		{
			struct FProcessEventMemory
			{
				FHeartNodeSource NodeSource;
				bool RetVal;
			};

			return Algo::AllOf(ScriptFilters,
				[Value](const FScriptDelegate& Delegate)
				{
					FProcessEventMemory Memory{Value.Source};
					Delegate.ProcessDelegate<UObject>(&Memory);
					return Memory.RetVal;
				});
		});

	if (ScriptSort.IsBound())
	{
		switch (SortMode)
		{
		case Comparison:
			Query.Sort(
				[&](const Heart::Query::FRegistryKey A, const Heart::Query::FRegistryKey B)
				{
					struct FProcessEventMemory
					{
						FHeartNodeSource NodeSourceA;
						FHeartNodeSource NodeSourceB;
						bool RetVal;
					};
					FProcessEventMemory Memory{Query[A].Source, Query[B].Source};
					ScriptSort.ProcessDelegate<UObject>(&Memory);
					return Memory.RetVal;
				});
			break;
		case Score:
			{
				Query.SortBy(
					[&](const Heart::Query::FRegistryKey A)
					{
						struct FProcessEventMemory
						{
							FHeartNodeSource NodeSource;
							double RetVal;
						};
						FProcessEventMemory Memory{Query[A].Source};
						ScriptSort.ProcessDelegate<UObject>(&Memory);
						return Memory.RetVal;
					},
					TGreater<>());
			}
			break;
		case Default:
			Query.Sort();
		case Off:
			break;
		}
	}

	Results.Reserve(Query.Num());

	Query.ForEach(
		[&Results](const Heart::Query::FRegistryValue& Value)
		{
			Results.Emplace(Value.GraphNode.Get(), Value.Source);
		});
}

void UHeartRegistryQuery::AddFilter(const FHeartRegistryBlueprintFilter& Predicate)
{
	FScriptDelegate ScriptDelegate;
	ScriptDelegate.BindUFunction(const_cast<UObject*>(Predicate.GetUObject()), Predicate.GetFunctionName());
	ScriptFilters.Add(ScriptDelegate);
}

void UHeartRegistryQuery::ClearFilters()
{
	ScriptFilters.Empty();
}

void UHeartRegistryQuery::SetSortByComparison(const FHeartRegistryBlueprintSort& Predicate)
{
   	ScriptSort.BindUFunction(const_cast<UObject*>(Predicate.GetUObject()), Predicate.GetFunctionName());
	SortMode = ESortMode::Comparison;
}

void UHeartRegistryQuery::SetSortByScore(const FHeartRegistryBlueprintScore& Predicate)
{
	ScriptSort.BindUFunction(const_cast<UObject*>(Predicate.GetUObject()), Predicate.GetFunctionName());
	SortMode = ESortMode::Score;
}

void UHeartRegistryQuery::SetSortByDefault()
{
	ScriptSort.Clear();
	SortMode = ESortMode::Default;
}

void UHeartRegistryQuery::ClearSort()
{
	ScriptSort.Clear();
	SortMode = ESortMode::Off;
}