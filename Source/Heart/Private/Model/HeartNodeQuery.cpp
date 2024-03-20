// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartNodeQuery.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

namespace Heart::Query
{
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

	TNodeQueryResult<UHeartGraph>::TNodeQueryResult(const UHeartGraph* Src)
	  : Reference(Src) {}

	const FNodeMap& TNodeQueryResult<UHeartGraph>::SimpleData() const { return Reference->GetNodes(); }
}