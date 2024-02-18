// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartSceneNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSceneNode)

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