// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartExtensionQuery.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphExtension.h"

namespace Heart::Query
{
	TExtensionQueryResult<FExtensionMap>::TExtensionQueryResult(const TConstArrayView<TObjectPtr<UHeartGraphExtension>>& Src)
	  : Reference([Src]()
			{
				TMap<FHeartExtensionGuid, TObjectPtr<UHeartGraphExtension>> Loose;

				for (auto Element : Src)
				{
					Loose.Add(Element->GetGuid(), Element);
				}

				return Loose;
			}()) {}

	TExtensionQueryResult<UHeartGraph>::TExtensionQueryResult(const UHeartGraph* Src)
	  : Reference(Src) {}

	const FExtensionMap& TExtensionQueryResult<UHeartGraph>::SimpleData() const { return Reference->GetExtensions(); }
}