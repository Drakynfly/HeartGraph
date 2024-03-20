// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartPinQuery.h"
#include "Model/HeartPinData.h"

namespace Heart::Query
{
	FPinQueryResult::FPinQueryResult(const FHeartNodePinData& Src)
	  : Reference(Src) {}

	FPinQueryResult& FPinQueryResult::CustomSort()
	{
		return SortBy([this](const FHeartPinGuid& Key)
			{
				return Reference.PinOrder[Key];
			});
	}

	const TMap<FHeartPinGuid, FHeartGraphPinDesc>& FPinQueryResult::SimpleData() const
	{
		return Reference.PinDescriptions;
	}
}