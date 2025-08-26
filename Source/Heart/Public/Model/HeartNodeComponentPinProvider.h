// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphPinDesc.h"
#include "UObject/Interface.h"
#include "HeartNodeComponentPinProvider.generated.h"

UINTERFACE(Blueprintable)
class UHeartNodeComponentPinProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HEART_API IHeartNodeComponentPinProvider
{
	GENERATED_BODY()

public:
	virtual void GatherPins(TArray<FHeartGraphPinDesc>& Pins) const PURE_VIRTUAL(IHeartNodeComponentPinProvider::GatherPins, )
};
