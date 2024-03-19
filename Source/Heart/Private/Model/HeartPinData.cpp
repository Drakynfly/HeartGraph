// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartPinData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartPinData)

void FHeartNodePinData::AddPin(const FHeartPinGuid NewKey, const FHeartGraphPinDesc& Desc)
{
	PinOrder.Add(NewKey, PinOrder.Num());
	PinDescriptions.Add(NewKey, Desc);
}

bool FHeartNodePinData::RemovePin(const FHeartPinGuid Key)
{
	PinDescriptions.Remove(Key);
	PinConnections.Remove(Key);

	if (PinOrder.Contains(Key))
	{
		// @todo this is horrid
		const int32 PinIndex = PinOrder[Key];
		for (auto&& Element : PinOrder)
		{
			if (Element.Value > PinIndex)
			{
				Element.Value--;
			}
		}
	}

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
	return {};
}

FHeartGraphPinDesc& FHeartNodePinData::GetPinDesc(const FHeartPinGuid Key)
{
	return PinDescriptions.FindChecked(Key);
}

TOptional<FHeartGraphPinConnections> FHeartNodePinData::GetConnections(const FHeartPinGuid Key) const
{
	return PinConnections.Contains(Key) ? PinConnections[Key] : TOptional<FHeartGraphPinConnections>{};
}

FHeartGraphPinConnections& FHeartNodePinData::GetConnectionsMutable(const FHeartPinGuid Key)
{
	return PinConnections.FindOrAdd(Key);
}

const FHeartGraphPinConnections& FHeartNodePinData::GetConnections(const FHeartPinGuid Key)
{
	return PinConnections.FindChecked(Key);
}

bool FHeartNodePinData::RemoveConnection(const FHeartPinGuid Key, const FHeartGraphPinReference& Pin)
{
	if (FHeartGraphPinConnections* Connections = PinConnections.Find(Key))
	{
		const int32 Removed = Connections->Links.Remove(Pin);
		if (Connections->Links.IsEmpty())
		{
			PinConnections.Remove(Key);
		}
		return !!Removed;
	}

	return false;
}