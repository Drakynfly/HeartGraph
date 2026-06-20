// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNodeComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNodeComponent)

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
