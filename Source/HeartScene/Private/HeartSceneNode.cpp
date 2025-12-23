// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartSceneNode.h"
#include "HeartSceneGenerator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSceneNode)

UHeartSceneNode::UHeartSceneNode()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UHeartGraph* UHeartSceneNode::GetHeartGraph() const
{
	return Generator->GetHeartGraph_Implementation();
}

FHeartNodeGuid UHeartSceneNode::GetNodeGuid() const
{
	return GraphNode;
}

void UHeartSceneNode::NativeOnCreated()
{
	OnCreated();
}