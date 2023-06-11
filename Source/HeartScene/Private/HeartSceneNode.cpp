// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartSceneNode.h"

UHeartSceneNode::UHeartSceneNode()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UHeartGraphNode* UHeartSceneNode::GetHeartGraphNode() const
{
	return GraphNode.Get();
}

void UHeartSceneNode::NativeOnCreated()
{
	OnCreated();
}
