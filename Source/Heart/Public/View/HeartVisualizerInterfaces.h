// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartVisualizerInterfaces.generated.h"

class UHeartGraphNode;
class UHeartGraphPin;

// This class does not need to be modified.
UINTERFACE()
class HEART_API UGraphNodeVisualizerInterface : public UInterface
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
class HEART_API UGraphPinVisualizerInterface : public UInterface
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

	// Get a custom attachment position for the connection. By default it will be wherever the owning node/graph decides:
	// The default behavior for widgets is to use the center of the widget geometry.
	// The default behavior for actors is to use sockets matching the pin name. // @todo implement this
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|VisualizerInterfaces")
	bool GetCustomAttachmentPosition(FVector& Position) const;
};