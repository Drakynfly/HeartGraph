// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNodeComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNodeComponent)

UHeartGraph* UHeartGraphNodeComponent::GetGraph() const
{
	return GetOuterUHeartGraph();
}