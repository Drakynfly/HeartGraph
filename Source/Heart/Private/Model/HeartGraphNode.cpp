// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNode.h"
#include "Model/HeartEdNodeInterface.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNode)

#define LOCTEXT_NAMESPACE "HeartGraphNode"

void FHeartNodePinData::AddPin(const FHeartPinGuid NewKey, const FHeartGraphPinDesc& Desc)
{
	PinDescriptions.Add(NewKey, Desc);
	PinOrder.Add(NewKey);
}

bool FHeartNodePinData::RemovePin(const FHeartPinGuid Key)
{
	PinDescriptions.Remove(Key);
	PinConnections.Remove(Key);
	return !!PinOrder.Remove(Key);
}

bool FHeartNodePinData::Contains(const FHeartPinGuid Key) const
{
	return PinDescriptions.Contains(Key);
}

int32 FHeartNodePinData::GetPinIndex(const FHeartPinGuid Key) const
{
	return PinOrder[Key];
}

bool FHeartNodePinData::HasConnections(const FHeartPinGuid Key) const
{
	if (PinConnections.Contains(Key))
	{
		return !PinConnections[Key].Links.IsEmpty();
	}
	return false;
}

TOptional<FHeartGraphPinDesc> FHeartNodePinData::GetPinDesc(const FHeartPinGuid Pin) const
{
	if (Pin.IsValid() && PinDescriptions.Contains(Pin))
	{
		return PinDescriptions[Pin];
	}
	return Heart::Graph::InvalidPinDesc;
}

TOptional<FHeartGraphPinConnections> FHeartNodePinData::GetConnections(const FHeartPinGuid Key) const
{
	return PinConnections.Contains(Key) ? PinConnections[Key] : FHeartGraphPinConnections();
}

FHeartGraphPinConnections& FHeartNodePinData::GetConnections(const FHeartPinGuid Key)
{
	return PinConnections.FindOrAdd(Key);
}

UHeartGraphNode::UHeartGraphNode()
{
#if WITH_EDITOR
	// Unless specified otherwise, use the graph's Schema default style.
	GetHeartGraphNodeSparseClassData()->EditorSlateStyle = "GraphDefault";
#endif
}

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

PRAGMA_DISABLE_DEPRECATION_WARNINGS
	if (!PinDescriptions.IsEmpty())
	{
		for (auto&& Element : PinDescriptions)
		{
			PinData.AddPin(Element.Key, Element.Value);
		}
		PinDescriptions.Empty();
	}

	if (!PinConnections.IsEmpty())
	{
		for (auto&& Element : PinConnections)
		{
			PinData.GetConnections(Element.Key) = Element.Value;
		}
		PinConnections.Empty();
	}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

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

void UHeartGraphNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty &&
	   (PropertyChangedEvent.MemberProperty->HasMetaData(Heart::Graph::Metadata_TriggersReconstruct) ||
		GetPropertiesTriggeringNodeReconstruction().Contains(PropertyChangedEvent.GetPropertyName())))
	{
		if (EdGraphNodePointer)
		{
			EdGraphNodePointer->OnPropertyChanged();
		}
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
			if (EdGraphNodePointer)
			{
				EdGraphNodePointer->OnPropertyChanged();
			}
		}
	}
}

#endif

UHeartGraphNode* UHeartGraphNode::GetHeartGraphNode() const
{
	return const_cast<UHeartGraphNode*>(this);
}

FText UHeartGraphNode::GetDefaultNodeCategory(const FHeartNodeSource NodeSource) const
{
	if (!NodeSource.IsValid()) return FText();
	return GetNodeCategory(NodeSource.GetDefaultObject());
}

FText UHeartGraphNode::GetDefaultNodeTooltip(const FHeartNodeSource NodeSource) const
{
	if (!NodeSource.IsValid()) return FText();
	return GetNodeToolTip(NodeSource.GetDefaultObject());
}

FText UHeartGraphNode::GetNodeTitle_Implementation(const UObject* Node) const
{
	if (IsValid(Node))
    {
    	return FText::FromString(Node->GetName());
    }

    return LOCTEXT("GetNodeTitle_Invalid", "[Invalid NodeObject]");
}

FText UHeartGraphNode::GetPreviewNodeTitle_Implementation(const FHeartNodeSource NodeSource, EHeartPreviewNodeNameContext Context) const
{
	if (NodeSource.IsValid())
	{
		return FText::FromString(NodeSource.As<UObject>()->GetName());
	}
	return LOCTEXT("GetPreviewNodeTitle_Invalid", "[Invalid NodeSource]");
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

FLinearColor UHeartGraphNode::GetNodeTitleColor_Implementation(const UObject* Node)
{
	return FLinearColor::White;
}

void UHeartGraphNode::GetNodeMessages_Implementation(TArray<FHeartGraphNodeMessage>& Messages) const
{
}

FText UHeartGraphNode::GetInstanceTitle() const
{
	return GetNodeTitle(NodeObject);
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
	return CastChecked<UHeartGraph>(GetOuter());
}

TOptional<FHeartGraphPinDesc> UHeartGraphNode::GetPinDesc(const FHeartPinGuid& Pin) const
{
	return PinData.GetPinDesc(Pin);
}

FHeartGraphPinDesc UHeartGraphNode::GetPinDescChecked(const FHeartPinGuid& Pin) const
{
	if (auto Desc = PinData.GetPinDesc(Pin))
	{
		checkSlow(Desc.IsSet());
		return Desc.GetValue();
	}
	return Heart::Graph::InvalidPinDesc;
}

FHeartGraphPinDesc UHeartGraphNode::GetPinDesc(const FHeartPinGuid& Pin, bool) const
{
	return GetPinDescChecked(Pin);
}

FHeartGraphPinReference UHeartGraphNode::GetPinReference(const FHeartPinGuid& Pin) const
{
	return PinData.Contains(Pin) ? FHeartGraphPinReference{Guid, Pin} : FHeartGraphPinReference();
}

FHeartPinGuid UHeartGraphNode::GetPinByName(const FName& Name) const
{
	// @todo make TOptional the return type here

	auto&& RetVal = PinData.Find(
		[Name](const TPair<FHeartPinGuid, FHeartGraphPinDesc>& Desc) -> TOptional<FHeartPinGuid>
		{
			if (Desc.Value.Name == Name)
			{
				return Desc.Key;
			}
			return {};
		});

	if (RetVal.IsSet())
	{
		return RetVal.GetValue();
	}

	return FHeartPinGuid();
}

TArray<FHeartPinGuid> UHeartGraphNode::GetPinsOfDirection(const EHeartPinDirection Direction, const bool bSorted) const
{
	return FindPinsByDirection(Direction).Sort(bSorted).Result;
}

TArray<FHeartPinGuid> UHeartGraphNode::GetInputPins(const bool bSorted) const
{
	return GetPinsOfDirection(EHeartPinDirection::Input, bSorted);
}

TArray<FHeartPinGuid> UHeartGraphNode::GetOutputPins(const bool bSorted) const
{
	return GetPinsOfDirection(EHeartPinDirection::Output, bSorted);
}

TArray<FHeartGraphPinDesc> UHeartGraphNode::GetDynamicPins_Implementation() const
{
	return {};
}

void UHeartGraphNode::GetInstancedPinData_Implementation(EHeartPinDirection Direction, FHeartGraphPinTag& Tag,
	TArray<UHeartGraphPinMetadata*>& Metadata) const
{
}

bool UHeartGraphNode::HasConnections(const FHeartPinGuid& Pin) const
{
	return PinData.HasConnections(Pin);
}

TSet<UHeartGraphNode*> UHeartGraphNode::GetConnectedGraphNodes(const EHeartPinDirection Direction) const
{
	const UHeartGraph* Graph = GetGraph();
	if (!ensure(IsValid(Graph))) return {};

	TSet<UHeartGraphNode*> UniqueConnections;

	for (auto&& Pins = FindPinsByDirection(Direction);
		 auto&& Pin : Pins.Result)
	{
		auto&& Links = PinData.GetConnections(Pin);
		if (!Links.IsSet()) continue;

		for (auto&& Link : Links.GetValue().Links)
		{
			if (UHeartGraphNode* Node = Graph->GetNode(Link.NodeGuid))
			{
				UniqueConnections.Add(Node);
			}
		}
	}

	return UniqueConnections;
}


#if WITH_EDITOR
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

FHeartGraphPinConnections& UHeartGraphNode::GetLinks(const FHeartPinGuid& Pin)
{
	return PinData.GetConnections(Pin);
}

TOptional<FHeartGraphPinConnections> UHeartGraphNode::GetLinks(const FHeartPinGuid& Pin) const
{
	return PinData.GetConnections(Pin);
}

FHeartGraphPinConnections UHeartGraphNode::GetLinks(const FHeartPinGuid& Pin, bool) const
{
	if (auto&& Links = PinData.GetConnections(Pin);
		Links.IsSet())
	{
		return Links.GetValue();
	}
	return {};
}

FHeartNodePinData::FPinQueryResult UHeartGraphNode::FindPinsByDirection(EHeartPinDirection Direction) const
{
	return PinData.Query().Filter(
		[Direction](const TPair<FHeartPinGuid, FHeartGraphPinDesc>& Desc)
		{
			return EnumHasAnyFlags(Desc.Value.Direction, Direction);
		});
}

FHeartPinGuid UHeartGraphNode::AddPin(const FHeartGraphPinDesc& Desc)
{
	if (!ensure(Desc.IsValid()))
	{
		return FHeartPinGuid();
	}

	const FHeartPinGuid NewKey = FHeartPinGuid::New();

	PinData.AddPin(NewKey, Desc);

	OnNodePinsChanged.Broadcast(this);

	return NewKey;
}

bool UHeartGraphNode::RemovePin(const FHeartPinGuid& Pin)
{
	if (!ensure(Pin.IsValid()))
	{
		return false;
	}

	if (PinData.RemovePin(Pin))
	{
		OnNodePinsChanged.Broadcast(this);
		return true;
	}

	return false;
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
	//@todo this will dump everything in PinConnections. either we need to reconstruct while preserving FGuids, or just use FNames
	PinData = FHeartNodePinData();

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

void UHeartGraphNode::NotifyPinConnectionsChanged(const FHeartPinGuid& Pin)
{
	BP_OnConnectionsChanged(Pin);
	OnPinConnectionsChanged.Broadcast(Pin);
}

#undef LOCTEXT_NAMESPACE