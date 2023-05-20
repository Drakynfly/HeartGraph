// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGraphExtension.generated.h"

class UHeartGraph;

/**
 *
 */
UCLASS(Within = HeartGraph)
class HEART_API UHeartGraphExtension : public UObject
{
	GENERATED_BODY()

	friend UHeartGraph;

protected:
	virtual void PostExtensionAdded() {}
	virtual void PreExtensionRemove() {}
};
