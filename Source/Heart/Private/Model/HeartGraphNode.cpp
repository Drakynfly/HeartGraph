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
			if (GetGraph()->GetWorld())
			{
				return GetGraph()->GetWorld();
			}
		}
	}

	return Super::GetWorld();
}

void UHeartGraphNode::PostLoad()
{
	UObject::PostLoad();

	if (!IsTemplate())
	{
		if (!IsValid(NodeObject))
		{
			UE_LOG(LogHeartGraph, Error,
				TEXT("[%s]'s NodeObject failed to load. This should always be a valid object. This node will need to be deleted and recreated!"), *GetName())
		}

		TArray<FHeartPinGuid> InvalidPins;

		for (auto&& Pin : Pins)
		{
			if (!Pin.Key.IsValid())
			{
				UE_LOG(LogHeartGraph, Error,
					TEXT("[%s]'s has an invalid guid. It and the pin will be removed. Maybe check if this node is disconnected from something now!"), *GetName())
				InvalidPins.Add(Pin.Key);
			}

			if (!IsValid(Pin.Value))
			{
				UE_LOG(LogHeartGraph, Error,
					TEXT("[%s]'s has an invalid pin. It will be removed. Maybe check if this node is disconnected from something now!"), *GetName())
				InvalidPins.Add(Pin.Key);
			}
		}

		if (!InvalidPins.IsEmpty())
		{
			for (auto&& InvalidPin : InvalidPins)
			{
				Pins.Remove(InvalidPin);
			}

			OnNodePinsChanged.Broadcast(this);
			MarkPackageDirty();
		}
	}
}

#if WITH_EDITOR

void UHeartGraphNode::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	// Save the NodeObject with us in the editor
	UHeartGraphNode* This = CastChecked<UHeartGraphNode>(InThis);
	//Collector.AddReferencedObject(This->NodeObject, This);

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

FText UHeartGraphNode::GetDefaultNodeCategory(const UClass* NodeClass) const
{
	if (!IsValid(NodeClass)) return FText();
	return GetNodeCategory(NodeClass->GetDefaultObject());
}

FText UHeartGraphNode::GetDefaultNodeTooltip(const UClass* NodeClass) const
{
	if (!IsValid(NodeClass)) return FText();
	return GetNodeToolTip(NodeClass->GetDefaultObject());
}

FText UHeartGraphNode::GetNodeTitle_Implementation(const UObject* Node, EHeartNodeNameContext Context) const
{
	switch (Context)
	{
	case EHeartNodeNameContext::NodeInstance:
		if (Node)
		{
			return Node->GetClass()->GetDisplayNameText();
		}

		return LOCTEXT("GetNodeTitle_Invalid", "Invalid NodeObject!");
		break;
	case EHeartNodeNameContext::Default:
	case EHeartNodeNameContext::Palette:
	default:
		return GetClass()->GetDisplayNameText();
	}
}

FText UHeartGraphNode::GetNodeCategory_Implementation(const UObject* Node) const
{
	// There is no default category, return an empty text. It's up to the implementation of a graph to determine if the
	// graph node class or the node object class sets the category.
	return FText();
}

FText UHeartGraphNode::GetNodeToolTip_Implementation(const UObject* Node) const
{
	return Node->GetClass()->GetToolTipText();
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

UHeartGraphPin* UHeartGraphNode::GetPinByName(const FName& Name)
{
	for (auto&& Pin : Pins)
	{
		if (IsValid(Pin.Value))
		{
			if (Pin.Value->PinName == Name)
			{
				return Pin.Value;
			}
		}
	}

	return nullptr;
}

template <>
TArray<UHeartGraphPin*> UHeartGraphNode::GetPinsOfDirection<UHeartGraphPin>(const EHeartPinDirection Direction) const
{
	return GetPinsOfDirection(Direction);
}

TArray<UHeartGraphPin*> UHeartGraphNode::GetPinsOfDirection(const EHeartPinDirection Direction) const
{
	TArray<UHeartGraphPin*> ReturnPins;

	for (auto&& PinPair : Pins)
	{
		if (!ensure(IsValid(PinPair.Value)))
		{
			continue;
		}

		if (EnumHasAnyFlags(PinPair.Value->GetDirection(), Direction))
		{
			ReturnPins.Add(PinPair.Value);
		}
	}

	return ReturnPins;
}

TArray<UHeartGraphPin*> UHeartGraphNode::GetPinsOfDirectionByClass(const EHeartPinDirection Direction, const TSubclassOf<UHeartGraphPin> Class) const
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
	return GetPinsOfDirectionByClass(EHeartPinDirection::Input, Class);
}

TArray<UHeartGraphPin*> UHeartGraphNode::GetOutputPins(const TSubclassOf<UHeartGraphPin> Class) const
{
	return GetPinsOfDirectionByClass(EHeartPinDirection::Output, Class);
}

uint8 UHeartGraphNode::GetUserInputNum() const
{
	uint8 Result = 0;

	// @todo this is not super efficient. GetInputPins is already one loop, then we do another.
	// this should be replaced with predicate function filtering

	// @todo bonus for adding ability for any filter func/ exposed to BP for custom bp pin filters

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

	// @todo See above optimization

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

bool UHeartGraphNode::CanCreate_Editor() const
{
	if (GetOverrideCanCreateInEditor())
	{
		return GetCanCreateInEditor();
	}

	return CanCreate();
}
#endif

void UHeartGraphNode::SetLocation(const FVector2D& NewLocation)
{
	Location = NewLocation;
	OnNodeLocationChanged.Broadcast(this, Location);
}

bool UHeartGraphNode::CanUserAddInput_Implementation() const
{
	return false;
}

bool UHeartGraphNode::CanUserAddOutput_Implementation() const
{
	return false;
}

bool UHeartGraphNode::CanCreate_Implementation() const
{
	return true;
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
	if (!ensure(IsValid(Pin)))
	{
		return false;
	}

	return RemovePinByGuid(Pin->GetGuid());
}

bool UHeartGraphNode::RemovePinByGuid(const FHeartPinGuid Pin)
{
	if (!ensure(Pin.IsValid()))
	{
		return false;
	}

	auto&& Removed = Pins.Remove(Pin);
	if (Removed > 0)
	{
		OnNodePinsChanged.Broadcast(this);
	}

	return Removed > 0;
}

UHeartGraphPin* UHeartGraphNode::AddInstancePin(const EHeartPinDirection Direction)
{
	uint8 NumInstancesPinsForDirection = 0;

	switch (Direction)
	{
	case EHeartPinDirection::Input:
		if (!CanUserAddInput())
		{
			return nullptr;
		}

		NumInstancesPinsForDirection = GetUserInputNum();
		break;
	case EHeartPinDirection::Output:
		if (!CanUserAddOutput())
		{
			return nullptr;
		}

		NumInstancesPinsForDirection = GetUserOutputNum();
		break;
	default:
		return nullptr;
	}

	const FName PinName = *FString::FromInt(NumInstancesPinsForDirection + 1);

	FEditorScriptExecutionGuard EditorScriptExecutionGuard;
	auto&& InstancePin = CreatePin(PinName, Direction, GetInstancedPinType());

	AddPin(InstancePin);

	return InstancePin;
}

void UHeartGraphNode::RemoveInstancePin(EHeartPinDirection Direction)
{
	uint8 NumInstancesPinsForDirection = 0;

	switch (Direction)
	{
	case EHeartPinDirection::Input:
		NumInstancesPinsForDirection = GetUserInputNum();
		break;
	case EHeartPinDirection::Output:
		NumInstancesPinsForDirection = GetUserOutputNum();
		break;
	default:
		return;
	}

	const FName PinName = *FString::FromInt(NumInstancesPinsForDirection);

	RemovePinsByPredicate(Direction, [PinName](const UHeartGraphPin* Pin)
	{
		return Pin->PinName == PinName;
	});
}

void UHeartGraphNode::OnCreate()
{
	auto&& TemplateInputs = GetDefaultInputs();
	auto&& TemplateOutputs = GetDefaultOutputs();

	// Create inputs
	for (auto&& TemplateInput : TemplateInputs)
	{
		if (auto&& NewPin = CreatePin(TemplateInput.Key, EHeartPinDirection::Input, TemplateInput.Value))
		{
			AddPin(NewPin);
		}
	}

	// Create outputs
	for (auto&& TemplateOutput : TemplateOutputs)
	{
		if (auto&& NewPin = CreatePin(TemplateOutput.Key, EHeartPinDirection::Output, TemplateOutput.Value))
		{
			AddPin(NewPin);
		}
	}

	BP_OnCreate();
}

void UHeartGraphNode::NotifyPinConnectionsChanged(UHeartGraphPin* Pin)
{
	BP_OnConnectionsChanged(Pin);
	OnPinConnectionsChanged.Broadcast(Pin);
}

#undef LOCTEXT_NAMESPACE