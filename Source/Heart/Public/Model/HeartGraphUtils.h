// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartGraphUtils.generated.h"

class IHeartGraphInterface;
class IHeartGraphNodeInterface;
class IHeartGraphPinInterface;
class UHeartGraph;
class UHeartGraphNode;

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FHeartGraphNodePredicate, UHeartGraphNode*, Node);

/**
 *
 */
UCLASS()
class HEART_API UHeartGraphUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class))
	static UHeartGraphNode* FindNodeOfClass(TScriptInterface<IHeartGraphInterface> Graph, TSubclassOf<UHeartGraphNode> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	static UHeartGraphNode* FindNodeByPredicate(TScriptInterface<IHeartGraphInterface> Graph, const FHeartGraphNodePredicate& Predicate);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class))
	static TArray<UHeartGraphNode*> FindAllNodesOfClass(TScriptInterface<IHeartGraphInterface> Graph, TSubclassOf<UHeartGraphNode> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	static TArray<UHeartGraphNode*> FindAllNodesByPredicate(TScriptInterface<IHeartGraphInterface> Graph, const FHeartGraphNodePredicate& Predicate);

	// Gets the Heart Graph from an object representing a Heart Graph Node, and attempts to cast it to the requested class.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Graph", ExpandBoolAsExecs = "ReturnValue"))
	static bool GetGraphTyped(TScriptInterface<IHeartGraphNodeInterface> Node, TSubclassOf<UHeartGraph> Class, UHeartGraph*& Graph);

	// Gets the Heart Node from an object representing a Heart Graph Pin, and attempts to cast it to the requested class.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Node", ExpandBoolAsExecs = "ReturnValue"))
	static bool GetNodeTyped(TScriptInterface<IHeartGraphPinInterface> Pin, TSubclassOf<UHeartGraphNode> Class, UHeartGraphNode*& Node);
};
