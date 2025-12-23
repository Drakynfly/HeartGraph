// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartNetExtensionInterface.generated.h"

UINTERFACE()
class UHeartNetExtensionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Optional interface that UHeartGraphExtensions and UHeartGraphNodeComponent can implement to customize behavior over the network.
 */
// @todo rename this to IHeartNodeComponentInterface
class HEARTNET_API IHeartNetExtensionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|Net")
	bool ShouldReplicate();
};