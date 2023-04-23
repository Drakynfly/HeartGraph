// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNode.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphPin.h"
#include "Model/HeartGraphStatics.h"

#define LOCTEXT_NAMESPACE "HeartGraphNode"

UWorld* UHeartGraphNode::GetWorld() const
{
	if (!IsTemplate())
	{
		if (GetGraph())
		{
			UWorld* GraphWorld = GetGraph()->GetWorld();
			if (IsValid(GraphWorld))
			{
				return GraphWorld;
			}
		}
	}

	return Super::GetWorld();
}

void UHeartGraphNode::PostLoad()
{
	Super::PostLoad();

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
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty &&
	   (PropertyChangedEvent.MemberProperty->HasMetaData(Heart::Graph::Metadata_TriggersReconstruct) ||
		GetPropertiesTriggeringNodeReconstruction().Contains(PropertyChangedEvent.GetPropertyName())))
	{
		OnReconstructionRequested.ExecuteIfBound();
	}
}

void UHeartGraphNode::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (auto&& Head = PropertyChangedEvent.PropertyChain.GetHead())
	{
		const FProperty* Property = Head->GetValue();

		if (Property && (Property->HasMetaData(Heart::Graph::Metadata_TriggersReconstruct) ||
		   	GetPropertiesTriggeringNodeReconstruction().Contains(Property->GetFName())))
		{
			OnReconstructionRequested.ExecuteIfBound();
		}
	}
}

#endif

UHeartGraphNode* UHeartGraphNode::GetHeartGraphNode_Implementation() const
{
	return const_cast<UHeartGraphNode*>(this);
}

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

FText UHeartGraphNode::GetNodeTitle_Implementation(const UObject* Node, const EHeartNodeNameContext Context) const
{
	switch (Context)
	{
	case EHeartNodeNameContext::NodeInstance:
		if (Node)
		{
			return Node->GetClass()->GetDisplayNameText();
		}

		return LOCTEXT("GetNodeTitle_Invalid", "Invalid NodeObject!");
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

FText UHeartGraphNode::GetInstanceTitle() const
{
	if (IsValid(NodeObject))
	{
		return GetNodeTitle(NodeObject, EHeartNodeNameContext::NodeInstance);
	}
	return FText::AsCultureInvariant(TEXT("<<INVALID_INSTANCE>>"));
}

uint8 UHeartGraphNode::GetInstancedInputNum() const
{
	return InstancedInputs;
}

uint8 UHeartGraphNode::GetInstancedOutputNum() const
{
	return InstancedOutputs;
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
			if (Pin.Value->PinDesc.Name == Name)
			{
				return Pin.Value;
			}
		}
	}

	return nullptr;
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

		if (EnumHasAnyFlags(PinPair.Value->PinDesc.Direction, Direction))
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
			if (EnumHasAnyFlags(PinPair.Value->PinDesc.Direction, Direction))
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

TArray<FHeartGraphPinDesc> UHeartGraphNode::GetDynamicPins_Implementation() const
{
	return {};
}

void UHeartGraphNode::GetInstancedPinData_Implementation(EHeartPinDirection Direction, FHeartGraphPinTag& Tag,
	TArray<UHeartGraphPinMetadata*>& Metadata) const
{
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

UHeartGraphPin* UHeartGraphNode::CreatePin(const FHeartGraphPinDesc& Desc)
{
	return CreatePinOfClass(UHeartGraphPin::StaticClass(), Desc);
}

UHeartGraphPin* UHeartGraphNode::CreatePinOfClass(const TSubclassOf<UHeartGraphPin> Class, const FHeartGraphPinDesc& Desc)
{
	auto&& NewPin = NewObject<UHeartGraphPin>(this, Class);
	NewPin->Guid = FHeartPinGuid::NewGuid();
	NewPin->PinDesc = Desc;
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
	FHeartGraphPinDesc PinDesc;

	switch (Direction)
	{
	case EHeartPinDirection::Input:
		if (!CanUserAddInput())
		{
			return nullptr;
		}

		PinDesc.Name = *FString::FromInt(++InstancedInputs);
		PinDesc.Direction = EHeartPinDirection::Input;
		break;
	case EHeartPinDirection::Output:
		if (!CanUserAddOutput())
		{
			return nullptr;
		}

		PinDesc.Name = *FString::FromInt(++InstancedOutputs);
		PinDesc.Direction = EHeartPinDirection::Output;
		break;
	default:
		return nullptr;
	}

	{
		FEditorScriptExecutionGuard EditorScriptExecutionGuard;
		GetInstancedPinData(Direction, PinDesc.Tag, PinDesc.Metadata);
	}

	auto&& InstancePin = CreatePin(PinDesc);

	AddPin(InstancePin);

	return InstancePin;
}

void UHeartGraphNode::RemoveInstancePin(const EHeartPinDirection Direction)
{
	FName PinName;

	switch (Direction)
	{
	case EHeartPinDirection::Input:
		PinName = *FString::FromInt(InstancedInputs--);
		break;
	case EHeartPinDirection::Output:
		PinName = *FString::FromInt(InstancedOutputs--);
		break;
	default:
		return;
	}

	RemovePinsByPredicate(Direction, [PinName](const UHeartGraphPin* Pin)
	{
		return Pin->PinDesc.Name == PinName;
	});
}

void UHeartGraphNode::OnCreate()
{
	InstancedInputs = GetHeartGraphNodeSparseClassData()->DefaultInstancedInputs;
	InstancedOutputs = GetHeartGraphNodeSparseClassData()->DefaultInstancedOutputs;

	ReconstructPins();

	BP_OnCreate();
}

void UHeartGraphNode::ReconstructPins()
{
	Pins.Empty();

	auto&& DefaultInputs = GetDefaultInputs();
	auto&& DefaultOutputs = GetDefaultOutputs();
	auto&& DynamicPins = GetDynamicPins();

	// Create inputs
	for (auto&& TemplateInput : DefaultInputs)
	{
		if (auto&& NewPin = CreatePin(TemplateInput))
		{
			AddPin(NewPin);
		}
	}

	// Create outputs
	for (auto&& TemplateOutput : DefaultOutputs)
	{
		if (auto&& NewPin = CreatePin(TemplateOutput))
		{
			AddPin(NewPin);
		}
	}

	// Create dynamic inputs & outputs
	for (auto&& DynamicPin : DynamicPins)
	{
		if (auto&& NewPin = CreatePin(DynamicPin))
		{
			AddPin(NewPin);
		}
	}

	// Create instanced inputs
	for (int32 i = 0; i < InstancedInputs; ++i)
	{
		AddInstancePin(EHeartPinDirection::Input);
	}

	// Create instanced outputs
	for (int32 i = 0; i < InstancedOutputs; ++i)
	{
		AddInstancePin(EHeartPinDirection::Output);
	}
}

void UHeartGraphNode::NotifyPinConnectionsChanged(UHeartGraphPin* Pin)
{
	BP_OnConnectionsChanged(Pin);
	OnPinConnectionsChanged.Broadcast(Pin);
}

#undef LOCTEXT_NAMESPACE