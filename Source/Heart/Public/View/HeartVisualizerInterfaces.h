// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartVisualizerInterfaces.generated.h"

class UHeartGraphNode;
class UHeartGraphPin;

// This class does not need to be modified.
UINTERFACE()
class UGraphNodeVisualizerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class HEART_API IGraphNodeVisualizerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|VisualizerInterfaces")
	TSubclassOf<UHeartGraphNode> GetSupportedGraphNodeClass();
};

// This class does not need to be modified.
UINTERFACE()
class UGraphPinVisualizerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class HEART_API IGraphPinVisualizerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|VisualizerInterfaces")
	TSubclassOf<UHeartGraphPin> GetSupportedGraphPinClass();
};