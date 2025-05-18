// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNode.h"
#include "Model/HeartGraph.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNode)

#define LOCTEXT_NAMESPACE "HeartGraphNode"

UHeartGraphNode::UHeartGraphNode()
{
#if WITH_EDITOR
	// Unless specified otherwise, use the graph's Schema default style.
	if (auto SparseNodeData = GetMutableHeartGraphNodeSparseClassData();
		SparseNodeData && SparseNodeData->EditorSlateStyle.IsNone())
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

TConstStructView<FHeartGraphPinDesc> UHeartGraphNode::ViewPin(const FHeartPinGuid& Pin) const
{
	return PinData.ViewPin(Pin);
}

TOptional<FHeartGraphPinDesc> UHeartGraphNode::GetPinDesc(const FHeartPinGuid& Pin) const
{
	return PinData.GetPinDesc(Pin);
}

FHeartGraphPinDesc UHeartGraphNode::GetPinDescChecked(const FHeartPinGuid& Pin) const
{
	auto PinView = PinData.ViewPin(Pin);
	if (PinView.IsValid())
	{
		return PinView.Get();
	}
	return Heart::Graph::InvalidPinDesc;
}

bool UHeartGraphNode::IsPinOnNode(const FHeartPinGuid& Pin) const
{
	return PinData.Contains(Pin);
}

int32 UHeartGraphNode::GetPinCount() const
{
	return PinData.Num();
}

FHeartGraphPinReference UHeartGraphNode::GetPinReference(const FHeartPinGuid& Pin) const
{
	return PinData.Contains(Pin) ? FHeartGraphPinReference{Guid, Pin} : FHeartGraphPinReference();
}

FHeartPinGuid UHeartGraphNode::GetPinByName(FName Name) const
{
	auto&& RetVal = PinData.Find(
		[Name](const TPair<FHeartPinGuid, FHeartGraphPinDesc>& Desc) -> TOptional<FHeartPinGuid>
		{
			if (Desc.Value.Name == Name)
			{
				return Desc.Key;
			}
			return NullOpt;
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

TConstStructView<FHeartGraphPinConnections> UHeartGraphNode::ViewConnections(const FHeartPinGuid& Pin) const
{
	return PinData.ViewConnections(Pin);
}

bool UHeartGraphNode::FindConnections(const FHeartPinGuid& Pin, TArray<FHeartGraphPinReference>& Connections) const
{
	if (auto Links = PinData.ViewConnections(Pin);
		Links.IsValid())
	{
		Connections = Links.Get().GetLinks();
		return true;
	}
	return false;
}

TSet<FHeartGraphPinReference> UHeartGraphNode::GetConnections(const FHeartPinGuid& Pin, bool) const
{
	if (auto Links = PinData.ViewConnections(Pin);
		Links.IsValid())
	{
		return TSet<FHeartGraphPinReference>(TArray<FHeartGraphPinReference>(Links.Get().GetLinks()));
	}
	return {};
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

void UHeartGraphNode::SetGuid_Editor(const FGuid InGuid)
{
	Guid = *((FHeartNodeGuid*)&InGuid); // :(
}
#endif

void UHeartGraphNode::SetLocation(const FVector2D& NewLocation)
{
	Location = NewLocation;
	OnNodeLocationChanged_Native.Broadcast(this, Location);
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

	OnNodePinsChanged_Native.Broadcast(this);
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
		OnNodePinsChanged_Native.Broadcast(this);
		OnNodePinsChanged.Broadcast(this);
		return true;
	}

	return false;
}

FHeartGraphPinDesc UHeartGraphNode::MakeInstancedPin(const EHeartPinDirection Direction)
{
	FHeartGraphPinDesc PinDesc;

	switch (Direction)
	{
	case EHeartPinDirection::Input:
		if (!CanUserAddInput())
		{
			return Heart::Graph::InvalidPinDesc;
		}

		PinDesc.Name = *FString::FromInt(++InstancedInputs);
		PinDesc.Direction = EHeartPinDirection::Input;
		break;
	case EHeartPinDirection::Output:
		if (!CanUserAddOutput())
		{
			return Heart::Graph::InvalidPinDesc;
		}

		PinDesc.Name = *FString::FromInt(++InstancedOutputs);
		PinDesc.Direction = EHeartPinDirection::Output;
		break;
	default:
		return Heart::Graph::InvalidPinDesc;
	}

	{
#if WITH_EDITOR
		FEditorScriptExecutionGuard EditorScriptExecutionGuard;
#endif
		GetInstancedPinData(Direction, PinDesc.Tag, MutableView(PinDesc.Metadata));
	}

	return PinDesc;
}

FHeartPinGuid UHeartGraphNode::AddInstancePin(const EHeartPinDirection Direction)
{
	if (const FHeartGraphPinDesc Pin = MakeInstancedPin(Direction);
		Pin.IsValid())
	{
		return AddPin(Pin);
	}
	return FHeartPinGuid();
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

void UHeartGraphNode::OnCreate(UObject* NodeSpawningContext)
{
	InstancedInputs = GetDefaultInstancedInputs();
	InstancedOutputs = GetDefaultInstancedOutputs();

	ReconstructPins(true);

	BP_OnCreate(NodeSpawningContext);
}

bool UHeartGraphNode::ReconstructPins(const bool IsCreation)
{
	TArray<FHeartGraphPinDesc> GatheredPins;
	GatheredPins.Append(GetDefaultPins());
	GatheredPins.Append(CreateDynamicPins());

	// Instanced pins (@todo move to node component)
	{
		const uint8 InstancedInputNum = InstancedInputs;
		const uint8 InstancedOutputNum = InstancedOutputs;
		InstancedInputs = 0;
		InstancedOutputs = 0;

		// Create instanced inputs
		for (uint8 i = 0; i < InstancedInputNum; ++i)
		{
			GatheredPins.Add(MakeInstancedPin(EHeartPinDirection::Input));
		}

		// Create instanced outputs
		for (uint8 i = 0; i < InstancedOutputNum; ++i)
		{
			GatheredPins.Add(MakeInstancedPin(EHeartPinDirection::Output));
		}
	}

	if (IsCreation)
	{
		// Create all pins
		for (auto&& Pin : GatheredPins)
		{
			AddPin(Pin);
		}
		return true;
	}
	else
	{
		TArray<FHeartPinGuid> DiscardedPins;

		// Iterate over existing pins and remove the ones we already have from GatheredPins, or discard them
		for (auto&& ExistingPin : PinData.PinDescriptions)
		{
			bool Found = false;

			for (auto It = GatheredPins.CreateIterator(); It; ++It)
			{
				auto&& GatheredPin = *It;
				if (GatheredPin.Name == ExistingPin.Value.Name)
				{
					ExistingPin.Value = GatheredPin; // Overwrite anyway, to update other info that may have changed.
					// @todo should we do anything about metadata?
					It.RemoveCurrent();
					Found = true;
					break;
				}
			}

			if (!Found)
			{
				// A pin with this name was not gathered, remove it.
				DiscardedPins.Add(ExistingPin.Key);
			}
		}

		bool Modified = false;

		if (!GatheredPins.IsEmpty())
		{
			// The remaining pins in Gathered do not exist, but should
			for (auto&& GatheredPin : GatheredPins)
			{
				AddPin(GatheredPin);
			}

			Modified |= true;
		}

		if (!DiscardedPins.IsEmpty())
		{
			// These pins exist, but shouldn't
			for (auto&& DiscardedPin : DiscardedPins)
			{
				RemovePin(DiscardedPin);
			}

			Modified |= true;
		}

		return Modified;
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
	OnPinConnectionsChanged_Native.Broadcast(Pin);
	OnPinConnectionsChanged.Broadcast(Pin);
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

#undef LOCTEXT_NAMESPACE