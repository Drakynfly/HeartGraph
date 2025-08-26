// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "NodeComponents/HeartDynamicPinsComponent.h"

void UHeartDynamicPinsComponent::GatherPins(TArray<FHeartGraphPinDesc>& Pins) const
{
	Pins.Append(DynamicPins);
}
