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

	if (const int32* Order = PinOrder.Find(Key))
	{
		// @todo this is horrid
		for (auto&& Element : PinOrder)
		{
			if (Element.Value > *Order)
			{
				Element.Value--;
			}
		}
	}

	return !!PinOrder.Remove(Key);
}

int32 FHeartNodePinData::Num() const
{
	return PinDescriptions.Num();
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
	return PinConnections.Contains(Key);
}

TOptional<FHeartGraphPinDesc> FHeartNodePinData::GetPinDesc(const FHeartPinGuid Key) const
{
	if (const FHeartGraphPinDesc* Descriptions = PinDescriptions.Find(Key))
	{
		return *Descriptions;
	}
	return NullOpt;
}

TConstStructView<FHeartGraphPinDesc> FHeartNodePinData::ViewPin(const FHeartPinGuid Key) const
{
	if (const FHeartGraphPinDesc* Descriptions = PinDescriptions.Find(Key))
	{
		return *Descriptions;
	}
	return TConstStructView<FHeartGraphPinDesc>();
}

FHeartGraphPinDesc& FHeartNodePinData::GetPinChecked(const FHeartPinGuid Key)
{
	return PinDescriptions.FindChecked(Key);
}

TConstStructView<FHeartGraphPinConnections> FHeartNodePinData::ViewConnections(const FHeartPinGuid Key) const
{
	if (const FHeartGraphPinConnections* Connections = PinConnections.Find(Key))
	{
		return *Connections;
	}
	return TConstStructView<FHeartGraphPinConnections>{};
}

FHeartGraphPinConnections& FHeartNodePinData::GetConnectionsMutable(const FHeartPinGuid Key)
{
	return PinConnections.FindOrAdd(Key);
}

void FHeartNodePinData::AddConnection(const FHeartPinGuid Key, const FHeartGraphPinReference& Pin)
{
	PinConnections.FindOrAdd(Key).Connections.AddUnique(Pin);
}

bool FHeartNodePinData::RemoveConnection(const FHeartPinGuid Key, const FHeartGraphPinReference& Pin)
{
	if (FHeartGraphPinConnections* Connections = PinConnections.Find(Key))
	{
		const int32 Removed = Connections->Connections.RemoveSingleSwap(Pin);

		// Remove Connections element, if empty.
		if (Connections->Connections.IsEmpty())
		{
			PinConnections.Remove(Key);
		}

		return !!Removed;
	}

	return false;
}