// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartLayoutHelper.h"
#include "Model/HeartGraph.h"
#include "ModelView/HeartNodeLocationAccessor.h"

void UHeartLayoutHelper::Layout(IHeartNodeLocationAccessor* Accessor)
{
	auto&& Graph = Accessor->GetHeartGraph();
	if (IsValid(Graph))
	{
		TArray<UHeartGraphNode*> AllNodes;
		Graph->GetNodeArray(AllNodes);
		Layout(Accessor, AllNodes);
	}
}
