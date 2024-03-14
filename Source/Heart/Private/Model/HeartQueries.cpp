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

	FPinQueryResult& FPinQueryResult::DefaultSort()
	{
		return SortBy([this](const FHeartPinGuid& Key)
			{
				return Reference.PinOrder[Key];
			});
	}

	TNodeQueryResult<FNodeMap>::TNodeQueryResult(const TConstArrayView<TObjectPtr<UHeartGraphNode>>& Src)
		: Reference([Src]()
		{
			TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>> Loose;

			for (auto Element : Src)
			{
				Loose.Add(Element->GetGuid(), Element);
			}

			return Loose;
		}()) {}

	TNodeQueryResult<UHeartGraph*>::TNodeQueryResult(const UHeartGraph* Src)
	  : Reference(Src) {}
}