// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.h"
#include "HeartQueries.h"

class UHeartGraph;
class UHeartGraphNode;

namespace Heart::Query
{
	// Data-type queries by node queries
	using FNodeMap = TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>>;

	// Stub for node query templates
	template <typename Source>
	class TNodeQueryResult
	{
	};

	// Specialization for loose arrays of nodes. May be from multiple graphs, or a subset of just one.
	template<>
	class HEART_API TNodeQueryResult<FNodeMap> : public
		TMapQueryBase<TNodeQueryResult<FNodeMap>, FHeartNodeGuid, TObjectPtr<UHeartGraphNode>>
	{
		friend TMapQueryBase;

	public:
		TNodeQueryResult(const TConstArrayView<TObjectPtr<UHeartGraphNode>>& Src);

		const FNodeMap& SimpleData() const { return Reference; }

	private:
		const FNodeMap Reference;
	};

	// Specialization for querying all nodes in a graph
	template<>
	class HEART_API TNodeQueryResult<UHeartGraph> : public
		TMapQueryBase<TNodeQueryResult<UHeartGraph>, FHeartNodeGuid, TObjectPtr<UHeartGraphNode>>
	{
		friend TMapQueryBase;

	public:
		TNodeQueryResult(const UHeartGraph* Src);

		const FNodeMap& SimpleData() const;

	private:
		const UHeartGraph* Reference;
	};

	using FGraphNodeQuery = TNodeQueryResult<UHeartGraph>;
}