// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNode.h"
#include "Model/HeartGraph.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNode)

#define LOCTEXT_NAMESPACE "HeartGraphNode"

UHeartGraphNode::UHeartGraphNode()
{
#if WITH_EDITOR
	// Unless specified otherwise, use the graph's Schema default style.
	if (auto SparseNodeData = GetHeartGraphNodeSparseClassData();
		SparseNodeData->EditorSlateStyle.IsNone())
	{
		SparseNodeData->EditorSlateStyle = "GraphDefault";
	}
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
			PinData.GetConnectionsMutable(Element.Key) = Element.Value;
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

UHeartGraphNode* UHeartGraphNode::GetHeartGraphNode() const
{
	return const_cast<ThisClass*>(this);
}

FText UHeartGraphNode::GetDefaultNodeCategory(const FHeartNodeSource& NodeSource) const
{
	if (!NodeSource.IsValid()) return FText::GetEmpty();
	return GetNodeCategory(NodeSource.GetDefaultObject());
}

FText UHeartGraphNode::GetDefaultNodeTooltip(const FHeartNodeSource& NodeSource) const
{
	if (!NodeSource.IsValid()) return FText::GetEmpty();
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
	return FText::GetEmpty();
}

FText UHeartGraphNode::GetNodeToolTip_Implementation(const UObject* Node) const
{
	return FText::GetEmpty();
}

FLinearColor UHeartGraphNode::GetNodeTitleColor_Implementation(const UObject* Node) const
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
	return FindPinsByDirection(Direction).SortIf(bSorted).Get();
}

TArray<FHeartPinGuid> UHeartGraphNode::GetInputPins(const bool bSorted) const
{
	return GetPinsOfDirection(EHeartPinDirection::Input, bSorted);
}

TArray<FHeartPinGuid> UHeartGraphNode::GetOutputPins(const bool bSorted) const
{
	return GetPinsOfDirection(EHeartPinDirection::Output, bSorted);
}

TArray<FHeartGraphPinDesc> UHeartGraphNode::CreateDynamicPins()
{
	checkSlow(!this->IsTemplate())
	return BP_GetDynamicPins();
}

void UHeartGraphNode::GetInstancedPinData_Implementation(EHeartPinDirection Direction, FHeartGraphPinTag& Tag,
	TArray<UHeartGraphPinMetadata*>& Metadata) const
{
}

bool UHeartGraphNode::HasConnections(const FHeartPinGuid& Pin) const
{
	return PinData.HasConnections(Pin);
}

bool UHeartGraphNode::FindConnections(const FHeartPinGuid& Pin, TArray<FHeartGraphPinReference>& Connections) const
{
	if (auto Links = PinData.ViewConnections(Pin);
		Links.IsValid())
	{
		Connections = Links->GetLinks();

		return true;
	}
	return false;
}

TOptional<FHeartGraphPinConnections> UHeartGraphNode::GetConnections(const FHeartPinGuid& Pin) const
{
	if (auto Links = PinData.ViewConnections(Pin);
		Links.IsValid())
	{
		return Links.Get();
	}
	return {};
}

TSet<FHeartGraphPinReference> UHeartGraphNode::GetConnections(const FHeartPinGuid& Pin, bool) const
{
	if (auto Links = PinData.ViewConnections(Pin);
		Links.IsValid())
	{
		return TSet<FHeartGraphPinReference>(TArray<FHeartGraphPinReference>(Links->GetLinks()));
	}
	return {};
}

TSet<UHeartGraphNode*> UHeartGraphNode::GetConnectedGraphNodes(const EHeartPinDirection Direction) const
{
	const UHeartGraph* Graph = GetGraph();
	if (!ensure(IsValid(Graph))) return {};

	TSet<UHeartGraphNode*> UniqueConnections;

	FindPinsByDirection(Direction).ForEach(
		[&](const FHeartPinGuid PinGuid)
		{
			auto&& Links = PinData.ViewConnections(PinGuid);
			if (!Links.IsValid())
			{
				return;
			}

			for (auto&& Link : Links.Get())
			{
				if (UHeartGraphNode* Node = Graph->GetNode(Link.NodeGuid))
				{
					UniqueConnections.Add(Node);
				}
			}
		});

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
	return PinData.GetConnectionsMutable(Pin);
}

Heart::Query::FPinQueryResult UHeartGraphNode::FindPinsByDirection(EHeartPinDirection Direction) const
{
	return Heart::Query::FPinQueryResult(PinData).Filter(
		[Direction](const FHeartGraphPinDesc& Desc)
		{
			return EnumHasAnyFlags(Desc.Direction, Direction);
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
#if WITH_EDITOR
		FEditorScriptExecutionGuard EditorScriptExecutionGuard;
#endif
		GetInstancedPinData(Direction, PinDesc.Tag, MutableView(PinDesc.Metadata));
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
	InstancedInputs = GetDefaultInstancedInputs();
	InstancedOutputs = GetDefaultInstancedOutputs();

	ReconstructPins();

	BP_OnCreate();
}

void UHeartGraphNode::ReconstructPins()
{
	//@todo this will dump everything in PinConnections. either we need to reconstruct while preserving FGuids, or just use FNames
	PinData = FHeartNodePinData();

	TArray<FHeartGraphPinDesc> DefaultPins = GetDefaultPins();
	TArray<FHeartGraphPinDesc> DynamicPins = CreateDynamicPins();
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
	{
#if WITH_EDITOR
		FEditorScriptExecutionGuard ScriptExecutionGuard;
#endif
		BP_OnConnectionsChanged(Pin);
	}
	OnPinConnectionsChanged.Broadcast(Pin);
}

#undef LOCTEXT_NAMESPACE