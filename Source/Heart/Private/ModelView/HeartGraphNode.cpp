// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartGraphNode.h"

#include "Model/HeartGraph.h"
#include "ModelView/HeartGraphPin.h"

UClass* UHeartGraphNode::GetSupportedClass_Implementation() const
{
	return nullptr;
}

UWorld* UHeartGraphNode::GetWorld() const
{
	if (GetGraph())
	{
		return GetGraph()->GetWorld();
	}
	return nullptr;
}

void UHeartGraphNode::NewGuid()
{
	Guid = FHeartNodeGuid::NewGuid();
}

UHeartGraph* UHeartGraphNode::GetGraph() const
{
	return GetOwningGraph<UHeartGraph>();
}

void UHeartGraphNode::SetLocation(const FVector2D& NewLocation)
{
	Location = FVector(NewLocation.X, NewLocation.Y, 0);
}

void UHeartGraphNode::SetLocation3D(const FVector& NewLocation)
{
	Location = NewLocation;
}

UHeartGraphPin* UHeartGraphNode::GetPin(const FHeartPinGuid& PinGuid)
{
	auto Result = Pins.Find(PinGuid);
	return Result ? *Result : nullptr;
}

void UHeartGraphNode::AddPin(UHeartGraphPin* Pin)
{
	if (!ensure(IsValid(Pin) && Pin->GetGuid().IsValid()))
	{
		return;
	}

	Pins.Add(Pin->GetGuid() ,Pin);
}

bool UHeartGraphNode::RemovePin(UHeartGraphPin* Pin)
{
	if (!ensure(IsValid(Pin) && Pin->GetGuid().IsValid()))
	{
		return false;
	}

	const auto Removed = Pins.Remove(Pin->GetGuid());
	return Removed > 0;
}
