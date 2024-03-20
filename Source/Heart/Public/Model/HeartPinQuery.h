// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphPinDesc.h"
#include "HeartGuids.h"
#include "HeartQueries.h"

struct FHeartNodePinData;

namespace Heart::Query
{
	class HEART_API FPinQueryResult : public TMapQueryBase<FPinQueryResult, FHeartPinGuid, FHeartGraphPinDesc>
	{
		friend TMapQueryBase;

	public:
		FPinQueryResult(const FHeartNodePinData& Src);

		// Sort the results by their Pin Order
		FPinQueryResult& CustomSort();

		const TMap<FHeartPinGuid, FHeartGraphPinDesc>& SimpleData() const;

	private:
		const FHeartNodePinData& Reference;
	};
}