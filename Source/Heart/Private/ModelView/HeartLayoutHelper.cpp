// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartLayoutHelper.h"
#include "ModelView/HeartNodeLocationAccessor.h"
#include "Model/HeartGraph.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartLayoutHelper)

bool UHeartLayoutHelper::Layout(IHeartNodeLocationAccessor* Accessor) const
{
	const UHeartGraph* Graph = Accessor->GetHeartGraph();
	if (IsValid(Graph))
	{
		TArray<UHeartGraphNode*> AllNodes;
		Graph->GetNodeArray(AllNodes);
		return Layout(Accessor, AllNodes);
	}

	return false;
}

bool UHeartLayoutHelper_BlueprintBase::Layout(IHeartNodeLocationAccessor* Accessor,
	const TArray<UHeartGraphNode*>& Nodes) const
{
	return Layout_BP(Accessor->_getUObject(), Nodes);
}