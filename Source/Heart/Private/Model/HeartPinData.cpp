// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartPinData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartPinData)

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

FHeartGraphPinDesc& FHeartNodePinData::GetPinDesc(const FHeartPinGuid Key)
{
	return PinDescriptions.FindChecked(Key);
}

TOptional<FHeartGraphPinConnections> FHeartNodePinData::GetConnections(const FHeartPinGuid Key) const
{
	return PinConnections.Contains(Key) ? PinConnections[Key] : FHeartGraphPinConnections();
}

FHeartGraphPinConnections& FHeartNodePinData::GetConnections(const FHeartPinGuid Key)
{
	return PinConnections.FindOrAdd(Key);
}