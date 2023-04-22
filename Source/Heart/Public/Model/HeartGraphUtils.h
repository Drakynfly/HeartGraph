// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartGraphUtils.generated.h"

class IHeartGraphNodeInterface;
class IHeartGraphPinInterface;
class UHeartGraph;
class UHeartGraphNode;

/**
 *
 */
UCLASS()
class HEART_API UHeartGraphUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Gets the Heart Graph from an object representing a Heart Graph Node, and attempts to cast it to the requested class.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Graph", ExpandBoolAsExecs = "ReturnValue"))
	static bool GetGraphTyped(TScriptInterface<IHeartGraphNodeInterface> Node, TSubclassOf<UHeartGraph> Class, UHeartGraph*& Graph);

	// Gets the Heart Node from an object representing a Heart Graph Pin, and attempts to cast it to the requested class.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Node", ExpandBoolAsExecs = "ReturnValue"))
	static bool GetNodeTyped(TScriptInterface<IHeartGraphPinInterface> Pin, TSubclassOf<UHeartGraphNode> Class, UHeartGraphNode*& Node);
};
