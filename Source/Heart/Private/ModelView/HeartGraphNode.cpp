// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartGraphNode.h"

#include "Model/HeartGraph.h"
#include "ModelView/HeartGraphPin.h"

UWorld* UHeartGraphNode::GetWorld() const
{
	if (GetGraph())
	{
		return GetGraph()->GetWorld();
	}
	return nullptr;
}

UClass* UHeartGraphNode::GetSupportedClass_Implementation() const
{
	return nullptr;
}

UHeartGraph* UHeartGraphNode::GetGraph() const
{
	return GetOwningGraph<UHeartGraph>();
}

UHeartGraphPin* UHeartGraphNode::GetPin(const FHeartPinGuid& PinGuid)
{
	auto&& Result = Pins.Find(PinGuid);
	return Result ? *Result : nullptr;
}

TArray<UHeartGraphPin*> UHeartGraphNode::GetPinsOfDirection(const EHeartPinDirection Direction, const TSubclassOf<UHeartGraphPin> Class) const
{
	TArray<UHeartGraphPin*> ReturnPins;

	for (auto&& PinPair : Pins)
	{
		if (!ensure(IsValid(PinPair.Value)))
		{
			continue;
		}

		if (PinPair.Value.IsA(Class))
		{
			if (PinPair.Value->GetDirection() == Direction)
			{
				ReturnPins.Add(PinPair.Value);
			}
		}
	}

	return ReturnPins;
}

TArray<UHeartGraphPin*> UHeartGraphNode::GetInputPins(const TSubclassOf<UHeartGraphPin> Class) const
{
	return GetPinsOfDirection(EHeartPinDirection::Input, Class);
}

TArray<UHeartGraphPin*> UHeartGraphNode::GetOutputPins(const TSubclassOf<UHeartGraphPin> Class) const
{
	return GetPinsOfDirection(EHeartPinDirection::Output, Class);
}

void UHeartGraphNode::NotifyPinConnectionsChanged(UHeartGraphPin* Pin)
{
	OnPinConnectionsChanged.Broadcast(Pin);
}

void UHeartGraphNode::SetLocation(const FVector2D& NewLocation)
{
	Location = NewLocation;
	OnNodeLocationChanged.Broadcast(this, Location);
}

UHeartGraphPin* UHeartGraphNode::CreatePin(const TSubclassOf<UHeartGraphPin> Class, const EHeartPinDirection Direction)
{
	auto&& NewPin = NewObject<UHeartGraphPin>(this, Class);
	NewPin->Guid = FHeartPinGuid::NewGuid();
	NewPin->PinDirection = Direction;
	return NewPin;
}

void UHeartGraphNode::AddPin(UHeartGraphPin* Pin)
{
	if (!ensure(IsValid(Pin) && Pin->GetGuid().IsValid()))
	{
		return;
	}

	Pins.Add(Pin->GetGuid() ,Pin);
	OnNodePinsChanged.Broadcast(this);
}

bool UHeartGraphNode::RemovePin(UHeartGraphPin* Pin)
{
	if (!ensure(IsValid(Pin) && Pin->GetGuid().IsValid()))
	{
		return false;
	}

	auto&& Removed = Pins.Remove(Pin->GetGuid());
	if (Removed > 0)
	{
		OnNodePinsChanged.Broadcast(this);
	}

	return Removed > 0;
}