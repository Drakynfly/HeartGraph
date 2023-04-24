// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Model/HeartGraphNodeInterface.h"
#include "Model/HeartGraphPinInterface.h"
#include "Model/HeartGraphPinTag.h"
#include "HeartVisualizerInterfaces.generated.h"

UINTERFACE(Blueprintable)
class HEART_API UGraphNodeVisualizerInterface : public UHeartGraphNodeInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class HEART_API IGraphNodeVisualizerInterface : public IHeartGraphNodeInterface
{
	GENERATED_BODY()

protected:
	// Defer to Blueprint implementation by default
	virtual UHeartGraphNode* GetHeartGraphNode() const override
	{
		return Execute_GetVisualizingNode(_getUObject());
	}

public:
	// Get the Heart Graph Node that this object visualizes
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|Graph")
	UHeartGraphNode* GetVisualizingNode() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Heart|VisualizerInterfaces")
	TSubclassOf<UHeartGraphNode> GetSupportedGraphNodeClass();
};

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class HEART_API UGraphPinVisualizerInterface : public UHeartGraphPinInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class HEART_API IGraphPinVisualizerInterface : public IHeartGraphPinInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|VisualizerInterfaces")
	FHeartGraphPinTag GetSupportedGraphPinTag();

	// Get a custom attachment position for the connection. By default it will be wherever the owning node/graph decides:
	// The default behavior for widgets is to use the center of the widget geometry.
	// The default behavior for actors is to use sockets matching the pin name. // @todo implement this
	// If relative is true, then the position will be interpreted as relative to the middle of the node.
	// @todo, maybe add pivot functionality similar to dragdropops, for placement around node easier
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Heart|VisualizerInterfaces")
	bool GetCustomAttachmentPosition(FVector& Position, bool& Relative) const;
};

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class HEART_API UGraphConnectionVisualizerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class HEART_API IGraphConnectionVisualizerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|VisualizerInterfaces")
	FHeartGraphPinTag GetSupportedGraphPinTag();
};
