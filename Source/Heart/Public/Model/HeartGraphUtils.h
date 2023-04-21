// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartGraphUtils.generated.h"

class UHeartGraph;
class UHeartGraphNode;
class UHeartGraphPin;

/**
 *
 */
UCLASS()
class HEART_API UHeartGraphUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Graph", ExpandBoolAsExecs = "ReturnValue"))
	static bool GetGraphTyped(UHeartGraphNode* Node, TSubclassOf<UHeartGraph> Class, UHeartGraph*& Graph);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Node", ExpandBoolAsExecs = "ReturnValue"))
	static bool GetNodeTyped(UHeartGraphPin* Pin, TSubclassOf<UHeartGraphNode> Class, UHeartGraphNode*& Node);
};
