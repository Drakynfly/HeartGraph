// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartCanvasUtils.generated.h"

class UHeartGraph;
class UHeartGraphNode;
class UHeartGraphCanvas;
class UHeartGraphCanvasNode;

/**
 *
 */
UCLASS()
class HEARTCANVAS_API UHeartCanvasUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvas", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Graph", ExpandBoolAsExecs = "ReturnValue"))
	static bool GetGraphTyped(UHeartGraphCanvas* Canvas, TSubclassOf<UHeartGraph> Class, UHeartGraph*& Graph);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphCanvasNode", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Graph", ExpandBoolAsExecs = "ReturnValue"))
	static bool GetNodeTyped(UHeartGraphCanvasNode* CanvasNode, TSubclassOf<UHeartGraphNode> Class, UHeartGraphNode*& Node);
};
