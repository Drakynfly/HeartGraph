// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNodeComponent.h"

UHeartGraph* UHeartGraphNodeComponent::GetGraph() const
{
	return GetOuterUHeartGraph();
}