// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "NodeComponents/HeartDynamicPinsComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartDynamicPinsComponent)

void UHeartDynamicPinsComponent::GatherPins(TArray<FHeartGraphPinDesc>& Pins) const
{
	Pins.Append(DynamicPins);
}
