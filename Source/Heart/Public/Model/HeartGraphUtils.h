// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GraphRegistry/HeartNodeSource.h"

#include "HeartGraphUtils.generated.h"

struct FHeartGuid;
struct FHeartGraphPinReference;
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
	UFUNCTION(BlueprintPure, Category = "Heart|Graph", meta = (DisplayName = "Equal (HeartGuid)", CompactNodeTitle = "==", ScriptMethod = "Equals", ScriptOperator = "==", Keywords = "== equal"))
	static bool Equal_HeartGuidHeartGuid(FHeartGuid A, FHeartGuid B);

	UFUNCTION(BlueprintPure, Category = "Heart|Graph", meta = (DisplayName = "Not Equal (HeartGuid)", CompactNodeTitle = "!=", ScriptMethod = "NotEqual", ScriptOperator = "!=", Keywords = "!= not equal"))
	static bool NotEqual_HeartGuidHeartGuid(FHeartGuid A, FHeartGuid B);

	UFUNCTION(BlueprintPure, Category = "Heart|Graph", meta = (DisplayName = "Equal (PinReference)", CompactNodeTitle = "==", ScriptMethod = "Equals", ScriptOperator = "==", Keywords = "== equal"))
	static bool Equal_PinReferencePinReference(FHeartGraphPinReference A, FHeartGraphPinReference B);

	UFUNCTION(BlueprintPure, Category = "Heart|Graph", meta = (DisplayName = "Not Equal (PinReference)", CompactNodeTitle = "!=", ScriptMethod = "NotEqual", ScriptOperator = "!=", Keywords = "!= not equal"))
	static bool NotEqual_PinReferencePinReference(FHeartGraphPinReference A, FHeartGraphPinReference B);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class))
	static UHeartGraphNode* FindNodeOfClass(const TScriptInterface<IHeartGraphInterface>& Graph, TSubclassOf<UHeartGraphNode> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	static UHeartGraphNode* FindNodeByPredicate(const TScriptInterface<IHeartGraphInterface>& Graph, const FHeartGraphNodePredicate& Predicate);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class))
	static TArray<UHeartGraphNode*> FindAllNodesOfClass(const TScriptInterface<IHeartGraphInterface>& Graph, TSubclassOf<UHeartGraphNode> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	static TArray<UHeartGraphNode*> FindAllNodesByPredicate(const TScriptInterface<IHeartGraphInterface>& Graph, const FHeartGraphNodePredicate& Predicate);


	// Gets the Heart Graph from an object representing a Heart Graph Node, and attempts to cast it to the requested class.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Graph", ExpandBoolAsExecs = "ReturnValue", DefaultToSelf = "Node"))
	static bool GetGraphTyped(const TScriptInterface<IHeartGraphNodeInterface>& Node, TSubclassOf<UHeartGraph> Class, UHeartGraph*& Graph);

	// Gets the Node Object from an object representing a Heart Graph Node, and attempts to cast it to the requested class.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Object", ExpandBoolAsExecs = "ReturnValue", DefaultToSelf = "Node"))
	static bool GetNodeObjectTyped(const TScriptInterface<IHeartGraphNodeInterface>& Node, TSubclassOf<UObject> Class, UObject*& Object);


	UFUNCTION(BlueprintPure, Category = "Heart|GraphPin")
	static FHeartGraphPinReference MakeReference(const TScriptInterface<IHeartGraphNodeInterface>& Node, const TScriptInterface<IHeartGraphPinInterface>& Pin);

	// Gets the Heart Node from an object representing a Heart Graph Pin, and attempts to cast it to the requested class.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Node", ExpandBoolAsExecs = "ReturnValue"))
	static bool GetGraphNodeTyped(const TScriptInterface<IHeartGraphPinInterface>& Pin, TSubclassOf<UHeartGraphNode> Class, UHeartGraphNode*& Node);


	// Makes a Node Source from a Class. Add this to a Registry to spawn nodes with instances of this class.
	UFUNCTION(BlueprintPure, Category = "Heart|Registry")
	static FHeartNodeSource MakeNodeSourceFromClass(UClass* Class);

	// Makes a Node Source from an Object. Add this to a Registry to spawn nodes with this object.
	UFUNCTION(BlueprintPure, Category = "Heart|Registry")
	static FHeartNodeSource MakeNodeSourceFromObject(UObject* Object);

	// Converts a Node Source to a class or returns nullptr
	UFUNCTION(BlueprintPure, Category = "Heart|Registry", meta = (DeterminesOutputType = "BaseClass"))
	static UClass* NodeSourceToClass(FHeartNodeSource NodeSource, const UClass* BaseClass);

	// Converts a Node Source to a object or returns nullptr
	UFUNCTION(BlueprintPure, Category = "Heart|Registry", meta = (DeterminesOutputType = "BaseClass"))
	static UObject* NodeSourceToObject(FHeartNodeSource NodeSource, const UClass* BaseClass);
};
