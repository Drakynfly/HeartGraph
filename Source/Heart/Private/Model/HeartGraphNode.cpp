// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNode.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphPin.h"

#define LOCTEXT_NAMESPACE "HeartGraphNode"

UWorld* UHeartGraphNode::GetWorld() const
{
	if (!IsTemplate())
	{
		if (GetGraph())
		{
			return GetGraph()->GetWorld();
		}
	}

	return nullptr;
}

void UHeartGraphNode::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	UObject::PostDuplicate(DuplicateMode);
}

#if WITH_EDITOR

void UHeartGraphNode::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	// Save the NodeObject with us in the editor
	UHeartGraphNode* This = CastChecked<UHeartGraphNode>(InThis);
	Collector.AddReferencedObject(This->NodeObject, This);

	Super::AddReferencedObjects(InThis, Collector);
}

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

UClass* UHeartGraphNode::GetSupportedClass_Implementation() const
{
	return nullptr;
}

FText UHeartGraphNode::GetDefaultNodeTitle_Implementation(const UObject* Node) const
{
	return FText::FromString(Node->GetName());
}

FText UHeartGraphNode::GetDefaultNodeCategory_Implementation(const UObject* Node) const
{
	return FText();
}

FText UHeartGraphNode::GetDefaultNodeToolTip_Implementation(const UObject* Node) const
{
	return FText();
}

FText UHeartGraphNode::GetNodeTitle_Implementation() const
{
	if (NodeObject)
	{
		return GetDefaultNodeTitle(NodeObject);
	}

	return LOCTEXT("GetNodeTitle_Invalid", "Invalid NodeObject!");
}

FText UHeartGraphNode::GetNodeCategory_Implementation() const
{
	if (NodeObject)
	{
		return GetDefaultNodeCategory(NodeObject);
	}

	return FText();
}

FText UHeartGraphNode::GetNodeToolTip_Implementation() const
{
	if (NodeObject)
	{
		return GetDefaultNodeToolTip(NodeObject);
	}

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

TMap<FName, FHeartGraphPinType> UHeartGraphNode::GetDynamicInputs_Implementation() const
{
	return TMap<FName, FHeartGraphPinType>();
}

TMap<FName, FHeartGraphPinType> UHeartGraphNode::GetDynamicOutputs_Implementation() const
{
	return TMap<FName, FHeartGraphPinType>();
}

FHeartGraphPinType UHeartGraphNode::GetInstancedPinType_Implementation()
{
	return FHeartGraphPinType();
}

#if WITH_EDITOR

void UHeartGraphNode::SetEdGraphNode(UEdGraphNode* GraphNode)
{
	if (HeartEdGraphNode != GraphNode)
	{
		HeartEdGraphNode = GraphNode;
	}
}

bool UHeartGraphNode::SupportsDynamicPins_Editor() const
{
	// Get editor sparse class member
	return GetSupportsDynamicPinsInEditor();
}

#endif

void UHeartGraphNode::SetLocation(const FVector2D& NewLocation)
{
	Location = NewLocation;
	OnNodeLocationChanged.Broadcast(this, Location);
}

bool UHeartGraphNode::SupportsDynamicPins_Runtime_Implementation() const
{
	return false;
}

bool UHeartGraphNode::CanUserAddInput_Implementation() const
{
	return false;
}

bool UHeartGraphNode::CanUserAddOutput_Implementation() const
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

UHeartGraphPin* UHeartGraphNode::CreatePin(const FName Name, const EHeartPinDirection Direction, const FHeartGraphPinType Type)
{
	return CreatePinOfClass(UHeartGraphPin::StaticClass(), Name, Direction, Type);
}

UHeartGraphPin* UHeartGraphNode::CreatePinOfClass(const TSubclassOf<UHeartGraphPin> Class, const FName Name, const EHeartPinDirection Direction, const FHeartGraphPinType Type)
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
		auto&& Pin = GetInputPins()[i];
		if (Pin->GetPinName() == PinName)
		{
			Pins.Remove(Pin->GetGuid());
			break;
		}
	}
}

void UHeartGraphNode::RemoveUserOutput(const FName& PinName)
{
	Modify();

	for (int32 i = 0; i < GetOutputPins().Num(); i++)
	{
		auto&& Pin = GetInputPins()[i];
		if (Pin->GetPinName() == PinName)
		{
			Pins.Remove(Pin->GetGuid());
			break;
		}
	}
}

void UHeartGraphNode::NotifyPinConnectionsChanged(UHeartGraphPin* Pin)
{
	OnPinConnectionsChanged.Broadcast(Pin);
}

#undef LOCTEXT_NAMESPACE