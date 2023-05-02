// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNode.h"
#include "Model/HeartGraph.h"
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

UHeartGraphNode* UHeartGraphNode::GetHeartGraphNode() const
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
			return FText::FromString(Node->GetClass()->GetName());
		}

		return LOCTEXT("GetNodeTitle_Invalid", "Invalid NodeObject!");
	case EHeartNodeNameContext::Default:
	case EHeartNodeNameContext::Palette:
	default:
		return FText::FromString(GetClass()->GetName());
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
	return FText();
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

FHeartGraphPinDesc UHeartGraphNode::GetPinDesc(const FHeartPinGuid Pin) const
{
	if (PinDescriptions.Contains(Pin))
	{
		return PinDescriptions[Pin];
	}
	return FHeartGraphPinDesc();
}

FHeartGraphPinReference UHeartGraphNode::GetPinReference(const FHeartPinGuid Pin) const
{
	if (PinDescriptions.Contains(Pin))
	{
		return {Guid, Pin};
	}
	return FHeartGraphPinReference();
}

FHeartPinGuid UHeartGraphNode::GetPinByName(const FName& Name) const
{
	for (auto&& Pin : PinDescriptions)
	{
		if (Pin.Value.Name == Name)
		{
			return Pin.Key;
		}
	}

	return FHeartPinGuid();
}

TArray<FHeartPinGuid> UHeartGraphNode::GetPinsOfDirection(const EHeartPinDirection Direction) const
{
	TArray<FHeartPinGuid> ReturnPins;

	for (auto&& PinPair : PinDescriptions)
	{
		if (EnumHasAnyFlags(PinPair.Value.Direction, Direction))
		{
			ReturnPins.Add(PinPair.Key);
		}
	}

	return ReturnPins;
}

TArray<FHeartPinGuid> UHeartGraphNode::GetInputPins() const
{
	return GetPinsOfDirection(EHeartPinDirection::Input);
}

TArray<FHeartPinGuid> UHeartGraphNode::GetOutputPins() const
{
	return GetPinsOfDirection(EHeartPinDirection::Output);
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

FHeartGraphPinConnections& UHeartGraphNode::GetLinks(const FHeartPinGuid Pin)
{
	return PinConnections.FindOrAdd(Pin);
}

FHeartGraphPinConnections UHeartGraphNode::GetLinks(const FHeartPinGuid Pin) const
{
	return PinConnections.Contains(Pin) ? PinConnections[Pin] : FHeartGraphPinConnections();
}

FHeartPinGuid UHeartGraphNode::AddPin(const FHeartGraphPinDesc& Desc)
{
	if (!ensure(Desc.IsValid()))
	{
		return FHeartPinGuid();
	}

	const FHeartPinGuid NewKey = FHeartPinGuid::NewGuid();

	PinDescriptions.Add(FHeartPinGuid::NewGuid(), Desc);

	OnNodePinsChanged.Broadcast(this);

	return NewKey;
}

bool UHeartGraphNode::RemovePin(const FHeartPinGuid Pin)
{
	if (!ensure(Pin.IsValid()))
	{
		return false;
	}

	auto&& Removed = PinDescriptions.Remove(Pin);
	PinConnections.Remove(Pin);
	if (Removed > 0)
	{
		OnNodePinsChanged.Broadcast(this);
	}

	return Removed > 0;
}

FHeartPinGuid UHeartGraphNode::AddInstancePin(const EHeartPinDirection Direction)
{
	FHeartGraphPinDesc PinDesc;

	switch (Direction)
	{
	case EHeartPinDirection::Input:
		if (!CanUserAddInput())
		{
			return FHeartPinGuid();
		}

		PinDesc.Name = *FString::FromInt(++InstancedInputs);
		PinDesc.Direction = EHeartPinDirection::Input;
		break;
	case EHeartPinDirection::Output:
		if (!CanUserAddOutput())
		{
			return FHeartPinGuid();
		}

		PinDesc.Name = *FString::FromInt(++InstancedOutputs);
		PinDesc.Direction = EHeartPinDirection::Output;
		break;
	default:
		return FHeartPinGuid();
	}

	{
		FEditorScriptExecutionGuard EditorScriptExecutionGuard;
		GetInstancedPinData(Direction, PinDesc.Tag, PinDesc.Metadata);
	}

	return AddPin(PinDesc);
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

	RemovePin(GetPinByName(PinName));
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
	PinDescriptions.Empty();

	//@todo this will dump everything in PinConnections. either we need to reconstruct while preserving FGuids, or just use FNames

	TArray<FHeartGraphPinDesc> DefaultPins = GetDefaultPins();
	TArray<FHeartGraphPinDesc> DynamicPins = GetDynamicPins();
	const uint8 InstancedInputNum = InstancedInputs;
	const uint8 InstancedOutputNum = InstancedOutputs;
	InstancedInputs = 0;
	InstancedOutputs = 0;

	// Create default inputs & outputs
	for (auto&& DefaultPin : DefaultPins)
	{
		AddPin(DefaultPin);
	}

	// Create dynamic inputs & outputs
	for (auto&& DynamicPin : DynamicPins)
	{
		AddPin(DynamicPin);
	}

	// Create instanced inputs
	for (uint8 i = 0; i < InstancedInputNum; ++i)
	{
		AddInstancePin(EHeartPinDirection::Input);
	}

	// Create instanced outputs
	for (uint8 i = 0; i < InstancedOutputNum; ++i)
	{
		AddInstancePin(EHeartPinDirection::Output);
	}
}

void UHeartGraphNode::NotifyPinConnectionsChanged(const FHeartPinGuid Pin)
{
	BP_OnConnectionsChanged(Pin);
	OnPinConnectionsChanged.Broadcast(Pin);
}

#undef LOCTEXT_NAMESPACE