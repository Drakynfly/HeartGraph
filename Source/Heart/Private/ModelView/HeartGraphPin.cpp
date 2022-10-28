// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartGraphPin.h"
#include "ModelView/HeartGraphNode.h"
#include "Model/HeartGraph.h"

UWorld* UHeartGraphPin::GetWorld() const
{
	if (GetNode())
	{
		return GetNode()->GetWorld();
	}
	return nullptr;
}

void UHeartGraphPin::NewGuid()
{
	Guid = FHeartPinGuid::NewGuid();
}

UHeartGraphNode* UHeartGraphPin::GetNode() const
{
	return GetOwningNode<UHeartGraphNode>();
}

FHeartGraphPinReference UHeartGraphPin::GetReference() const
{
	return { GetNode()->GetGuid(), Guid };
}

UHeartGraphPin* UHeartGraphPin::ResolveConnection(const int32 Index) const
{
	if (!ensure(Links.IsValidIndex(Index)))
	{
		return nullptr;
	}

	auto&& PinRef = Links[Index];

	if (auto&& Graph = GetNode()->GetGraph())
	{
		if (auto&& Node = Graph->GetNode(PinRef.NodeGuid))
		{
			if (auto&& Pin = Node->GetPin(PinRef.PinGuid))
			{
				return Pin;
			}
		}
	}

	return nullptr;
}
