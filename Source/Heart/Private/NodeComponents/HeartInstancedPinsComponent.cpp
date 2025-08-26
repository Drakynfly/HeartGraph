// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "NodeComponents/HeartInstancedPinsComponent.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

void UHeartInstancedPinsComponent::GatherPins(TArray<FHeartGraphPinDesc>& Pins) const
{
	// Make a single pin we can copy.
	FHeartGraphPinDesc PinDesc = MakeInstancedPin(EHeartPinDirection::Input, 0);

	// Create instanced inputs
	for (uint8 i = 0; i < InstancedInputs; ++i)
	{
		PinDesc.Name = *FString::FromInt(i);
		Pins.Add(PinDesc);
	}

	// Switch to output
	PinDesc.Direction = EHeartPinDirection::Output;

	// Create instanced outputs
	for (uint8 i = 0; i < InstancedOutputs; ++i)
	{
		PinDesc.Name = *FString::FromInt(i);
		Pins.Add(PinDesc);
	}
}

void UHeartInstancedPinsComponent::ResetPinCounts()
{
	InstancedInputs = 0;
	InstancedOutputs = 0;
}

FHeartGraphPinDesc UHeartInstancedPinsComponent::MakeInstancedPin(const EHeartPinDirection Direction, const int32 Index) const
{
	FHeartGraphPinDesc PinDesc;

	PinDesc.Name = *FString::FromInt(Index);
	PinDesc.Direction = Direction;
	PinDesc.Tag = PinTag;
	PinDesc.FriendlyName = PinFriendlyName;
	PinDesc.Tooltip = PinTooltip;
#if WITH_EDITOR
	PinDesc.EditorTooltip = PinEditorTooltip;
#endif
	PinDesc.Metadata = PinMetadata;

	return PinDesc;
}

FHeartPinGuid UHeartInstancedPinsComponent::AddInstancePin(const FHeartNodeGuid& Node, const EHeartPinDirection Direction)
{
	int32 NewPinIndex = 0;
	switch (Direction)
	{
	case EHeartPinDirection::Input:
		NewPinIndex = InstancedInputs++;
		break;
	case EHeartPinDirection::Output:
		NewPinIndex = InstancedInputs++;
		break;
	default:
		break;
	}

	if (const FHeartGraphPinDesc Pin = MakeInstancedPin(Direction, NewPinIndex);
		Pin.IsValid())
	{
		if (UHeartGraphNode* GraphNode = GetGraph()->GetNode(Node);
			IsValid(GraphNode))
		{
			return GraphNode->AddPin(Pin);
		}
	}
	return FHeartPinGuid();
}

FHeartPinGuid UHeartInstancedPinsComponent::RemoveInstancePin(const FHeartNodeGuid& Node, const EHeartPinDirection Direction)
{
	FName PinName;

	switch (Direction)
	{
	case EHeartPinDirection::Input:
		if (InstancedInputs == 0)
		{
			// Cannot remove, no instanced inputs
			return FHeartPinGuid();
		}
		PinName = *FString::FromInt(--InstancedInputs);
		break;
	case EHeartPinDirection::Output:
		if (InstancedOutputs == 0)
		{
			// Cannot remove, no instanced outputs
			return FHeartPinGuid();
		}
		PinName = *FString::FromInt(--InstancedOutputs);
		break;
	default:
		return FHeartPinGuid();
	}

	if (UHeartGraphNode* GraphNode = GetGraph()->GetNode(Node);
		IsValid(GraphNode))
	{
		if (const FHeartPinGuid PinGuid = GraphNode->GetPinByName(PinName);
			GraphNode->RemovePin(PinGuid))
		{
			return PinGuid;
		}
	}

	return FHeartPinGuid();
}
