// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartNetNodeInterface.generated.h"

UINTERFACE()
class UHeartNetNodeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Optional interface that UHeartGraphNodes can implement to customize behavior over the network.
 */
class HEARTNET_API IHeartNetNodeInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|NetNode")
	bool ShouldReplicate();
};