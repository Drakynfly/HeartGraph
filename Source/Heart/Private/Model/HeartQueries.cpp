// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Model/HeartQueries.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartPinData.h"

namespace Heart::Query
{
	FPinQueryResult::FPinQueryResult(const FHeartNodePinData& Src)
	  : Reference(Src) {}

	void FPinQueryResult::Internal_GetOptions(TArray<FHeartPinGuid>& Options) const
	{
		Reference.PinOrder.GetKeys(Options);
	}

	FPinQueryResult& FPinQueryResult::DefaultSort()
	{
		InitResults();

		Algo::SortBy(Results.GetValue(),
			[this](const FHeartPinGuid& Key)
			{
				return Reference.PinOrder[Key];
			});

		return *this;
	}

	const TMap<FHeartPinGuid, FHeartGraphPinDesc>& FPinQueryResult::Internal_GetMap() const
	{
		return Reference.PinDescriptions;
	}

	FNodeQueryResult::FNodeQueryResult(const UHeartGraph* Src)
	  : Reference(TInPlaceType<AsGraph>(), Src) {}

	FNodeQueryResult::FNodeQueryResult(const TConstArrayView<TObjectPtr<UHeartGraphNode>>& Src)
	  : Reference(TInPlaceType<AsLoose>(),
	  		[Src]()
			{
	  			AsLoose Loose;

				for (auto Element : Src)
				{
					Loose.Add(Element->GetGuid(), Element);
				}

				return Loose;
			}()) {}

	void FNodeQueryResult::Internal_GetOptions(TArray<FHeartNodeGuid>& Options) const
	{
		if (Reference.IsType<AsGraph>())
		{
			Reference.Get<AsGraph>()->GetNodeGuids(Options);
		}
		else if (Reference.IsType<AsLoose>())
		{
			Options.Reserve(Options.Num() + Reference.Get<AsLoose>().Num());
			for (auto&& Element : Reference.Get<AsLoose>())
			{
				Options.Emplace(Element.Key);
			}
		}
	}

	const TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>>& FNodeQueryResult::Internal_GetMap() const
	{
		if (Reference.IsType<AsGraph>())
		{
			return Reference.Get<AsGraph>()->GetNodes();
		}

		check(Reference.IsType<AsLoose>());
		return Reference.Get<AsLoose>();
	}
}