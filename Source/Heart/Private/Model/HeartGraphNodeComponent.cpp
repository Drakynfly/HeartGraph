// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNodeComponent.h"
#include "Model/HeartGraph.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNodeComponent)

UHeartGraph* UHeartGraphNodeComponent::GetGraph() const
{
	return GetTypedOuter<UHeartGraph>();
}

bool FHeartGraphNodeComponentMap::IsEmpty() const
{
	return Components.IsEmpty();
}

TObjectPtr<UHeartGraphNodeComponent> FHeartGraphNodeComponentMap::Find(const FHeartNodeGuid& Node) const
{
	if (auto&& NodePtr = Components.Find(Node))
	{
		return *NodePtr;
	}
	return nullptr;
}
