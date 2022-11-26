// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNode.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphPin.h"

UWorld* UHeartGraphNode::GetWorld() const
{
	if (GetGraph())
	{
		return GetGraph()->GetWorld();
	}
	return nullptr;
}

#if WITH_EDITOR
void UHeartGraphNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty &&
		PropertyChangedEvent.MemberProperty->HasMetaData("TriggerReconstruct"))
	{
		OnReconstructionRequested.Broadcast(this);
	}
}
#endif

FText UHeartGraphNode::GetNodeTitle_Implementation() const
{
	return FText::FromString(GetName());
}

UClass* UHeartGraphNode::GetSupportedClass_Implementation() const
{
	return nullptr;
}

FText UHeartGraphNode::GetNodeCategory_Implementation() const
{
	return FText();
}

FText UHeartGraphNode::GetNodeToolTip_Implementation() const
{
	return FText();
}

bool UHeartGraphNode::GetDynamicTitleColor_Implementation(FLinearColor& LinearColor)
{
	return false;
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
			if (EnumHasAnyFlags(PinPair.Value->GetDirection(), Direction))
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

uint8 UHeartGraphNode::GetUserInputNum() const
{
	uint8 Result = 0;

	auto&& Inputs = GetInputPins();
	for (auto&& Pin : Inputs)
	{
		if (Pin->GetPinName().ToString().IsNumeric())
		{
			Result++;
		}
	}
	return Result;
}

uint8 UHeartGraphNode::GetUserOutputNum() const
{
	uint8 Result = 0;

	auto&& Inputs = GetOutputPins();
	for (auto&& Pin : Inputs)
	{
		if (Pin->GetPinName().ToString().IsNumeric())
		{
			Result++;
		}
	}
	return Result;
}

TArray<UHeartGraphPin*> UHeartGraphNode::GetDynamicInputs_Implementation() const
{
	return TArray<UHeartGraphPin*>();
}

TArray<UHeartGraphPin*> UHeartGraphNode::GetDynamicOutputs_Implementation() const
{
	return TArray<UHeartGraphPin*>();
}

UHeartGraphPin* UHeartGraphNode::GetInstancedPin_Implementation()
{
	return nullptr;
}

#if WITH_EDITOR

void UHeartGraphNode::SetEdGraphNode(UEdGraphNode* GraphNode)
{
	if (HeartEdGraphNode != GraphNode)
	{
		HeartEdGraphNode = GraphNode;
	}
}
#endif

void UHeartGraphNode::SetLocation(const FVector2D& NewLocation)
{
	Location = NewLocation;
	OnNodeLocationChanged.Broadcast(this, Location);
}

bool UHeartGraphNode::SupportsDynamicPins_Implementation()
{
	return false;
}

bool UHeartGraphNode::CanUserAddInput_Implementation()
{
	return false;
}

bool UHeartGraphNode::CanUserAddOutput_Implementation()
{
	return false;
}

bool UHeartGraphNode::CanDelete_Implementation() const
{
	return true;
}

bool UHeartGraphNode::CanDuplicate_Implementation() const
{
	return true;
}

UHeartGraphPin* UHeartGraphNode::CreatePin(const TSubclassOf<UHeartGraphPin> Class, const FName Name, const EHeartPinDirection Direction)
{
	auto&& NewPin = NewObject<UHeartGraphPin>(this, Class);
	NewPin->Guid = FHeartPinGuid::NewGuid();
	NewPin->PinName = Name;
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

void UHeartGraphNode::RemoveUserInput(const FName& PinName)
{
	Modify();

	for (int32 i = 0; i < GetInputPins().Num(); i++)
	{
		if (GetInputPins()[i]->GetPinName() == PinName)
		{
			Pins.Remove(GetInputPins()[i]->GetGuid());
			break;
		}
	}
}

void UHeartGraphNode::RemoveUserOutput(const FName& PinName)
{
	Modify();

	for (int32 i = 0; i < GetOutputPins().Num(); i++)
	{
		if (GetOutputPins()[i]->GetPinName() == PinName)
		{
			Pins.Remove(GetOutputPins()[i]->GetGuid());
			break;
		}
	}
}

void UHeartGraphNode::NotifyPinConnectionsChanged(UHeartGraphPin* Pin)
{
	OnPinConnectionsChanged.Broadcast(Pin);
}
