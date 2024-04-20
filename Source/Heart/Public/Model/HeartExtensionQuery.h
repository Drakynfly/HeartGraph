// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.h"
#include "HeartQueries.h"

class UHeartGraph;
class UHeartGraphExtension;

namespace Heart::Query
{
	// Data-type queries by extension queries
	using FExtensionMap = TMap<FHeartExtensionGuid, TObjectPtr<UHeartGraphExtension>>;

	// Stub for extension query templates
	template <typename Source>
	class TExtensionQueryResult
	{
	};

	// Specialization for loose arrays of extensions. May be from multiple graphs, or a subset of just one.
	template<>
	class HEART_API TExtensionQueryResult<FExtensionMap> : public
		TMapQueryBase<TExtensionQueryResult<FExtensionMap>, FHeartExtensionGuid, TObjectPtr<UHeartGraphExtension>>
	{
		friend TMapQueryBase;

	public:
		TExtensionQueryResult(const TConstArrayView<TObjectPtr<UHeartGraphExtension>>& Src);

		const FExtensionMap& SimpleData() const { return Reference; }

	private:
		const FExtensionMap Reference;
	};

	// Specialization for querying all extensions in a graph
	template<>
	class HEART_API TExtensionQueryResult<UHeartGraph> : public
		TMapQueryBase<TExtensionQueryResult<UHeartGraph>, FHeartExtensionGuid, TObjectPtr<UHeartGraphExtension>>
	{
		friend TMapQueryBase;

	public:
		TExtensionQueryResult(const UHeartGraph* Src);

		const FExtensionMap& SimpleData() const;

	private:
		const UHeartGraph* Reference;
	};
}