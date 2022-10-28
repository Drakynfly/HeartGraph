// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGraphNode.h"
#include "HeartGraphBehavior.generated.h"

class UGraphNodeRegistrar;

/**
 * Allows graph classes to customize behavior and available functionality
 */
UCLASS(Abstract, Const, BlueprintType, Blueprintable)
class HEART_API UHeartGraphBehavior : public UObject
{
	GENERATED_BODY()

protected:
	/** Override to specify the node graph class for the graph class. Nodes added to the graph must inherit from it */
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|GraphBehavior")
	TSubclassOf<UHeartGraphNode> GetGraphNodeClass() const;
};