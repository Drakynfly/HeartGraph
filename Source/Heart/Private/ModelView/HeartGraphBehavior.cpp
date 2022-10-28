// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartGraphBehavior.h"

TSubclassOf<UHeartGraphNode> UHeartGraphBehavior::GetGraphNodeClass_Implementation() const
{
	return UHeartGraphNode::StaticClass();
}
