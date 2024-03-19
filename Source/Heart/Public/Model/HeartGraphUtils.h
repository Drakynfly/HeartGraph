// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GraphRegistry/HeartNodeSource.h"
#include "HeartPinData.h"

#include "HeartGraphUtils.generated.h"

struct FHeartGraphPinDesc;
struct FHeartGuid;
struct FHeartGraphPinReference;
class IHeartGraphInterface;
class IHeartGraphNodeInterface;
class IHeartGraphPinInterface;
class UHeartGraph;
class UHeartGraphNode;

namespace Heart::Utils
{
	using FFindNodePredicate = TDelegate<bool(const UHeartGraphNode*)>;

	[[nodiscard]] HEART_API UHeartGraphNode* FindNodeOfClass(const UHeartGraph* Graph, TSubclassOf<UHeartGraphNode> Class);

	[[nodiscard]] HEART_API UHeartGraphNode* FindNodeByPredicate(const UHeartGraph* Graph, const FFindNodePredicate& Predicate);

	[[nodiscard]] HEART_API TArray<UHeartGraphNode*> FindAllNodesOfClass(const UHeartGraph* Graph, TSubclassOf<UHeartGraphNode> Class);

	[[nodiscard]] HEART_API TArray<UHeartGraphNode*> FindAllNodesByPredicate(const UHeartGraph* Graph, const FFindNodePredicate& Predicate);

	[[nodiscard]] HEART_API Query::FPinQueryResult FindPinsByTag(const UHeartGraphNode* Node, FHeartGraphPinTag Tag);

	[[nodiscard]] HEART_API TOptional<FHeartGraphPinDesc> ResolvePinDesc(const UHeartGraph* Graph, const FHeartGraphPinReference& Reference);
}

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FHeartGraphNodePredicate, const UHeartGraphNode*, Node);

/**
 *
 */
UCLASS()
class HEART_API UHeartGraphUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**			STRUCT OPERATORS		*/

	UFUNCTION(BlueprintPure, Category = "Heart|Graph", meta = (DisplayName = "Equal (HeartGuid)", CompactNodeTitle = "==", ScriptMethod = "Equals", ScriptOperator = "==", Keywords = "== equal"))
	static bool Equal_HeartGuidHeartGuid(FHeartGuid A, FHeartGuid B);

	UFUNCTION(BlueprintPure, Category = "Heart|Graph", meta = (DisplayName = "Not Equal (HeartGuid)", CompactNodeTitle = "!=", ScriptMethod = "NotEqual", ScriptOperator = "!=", Keywords = "!= not equal"))
	static bool NotEqual_HeartGuidHeartGuid(FHeartGuid A, FHeartGuid B);

	UFUNCTION(BlueprintPure, Category = "Heart|Graph", meta = (DisplayName = "Equal (PinReference)", CompactNodeTitle = "==", ScriptMethod = "Equals", ScriptOperator = "==", Keywords = "== equal"))
	static bool Equal_PinReferencePinReference(const FHeartGraphPinReference& A, const FHeartGraphPinReference& B);

	UFUNCTION(BlueprintPure, Category = "Heart|Graph", meta = (DisplayName = "Not Equal (PinReference)", CompactNodeTitle = "!=", ScriptMethod = "NotEqual", ScriptOperator = "!=", Keywords = "!= not equal"))
	static bool NotEqual_PinReferencePinReference(const FHeartGraphPinReference& A, const FHeartGraphPinReference& B);


	/**			NODE ACCESSORS			*/

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class))
	static UHeartGraphNode* FindNodeOfClass(const TScriptInterface<IHeartGraphInterface>& Graph, TSubclassOf<UHeartGraphNode> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	static UHeartGraphNode* FindNodeByPredicate(const TScriptInterface<IHeartGraphInterface>& Graph, const FHeartGraphNodePredicate& Predicate);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class))
	static TArray<UHeartGraphNode*> FindAllNodesOfClass(const TScriptInterface<IHeartGraphInterface>& Graph, TSubclassOf<UHeartGraphNode> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	static TArray<UHeartGraphNode*> FindAllNodesByPredicate(const TScriptInterface<IHeartGraphInterface>& Graph, const FHeartGraphNodePredicate& Predicate);

	// Get all pins that match the tag.
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	static TArray<FHeartPinGuid> FindPinsByTag(const UHeartGraphNode* Node, FHeartGraphPinTag Tag);


	/**			NODE MISC UTILS			*/

	// Test if connecting two nodes would cause a loop in connections. Walks backwards through the inputs of A until it
	// finds or fails to find B.
	UFUNCTION(BlueprintPure, Category = "Heart|Graph")
	static bool WouldConnectionCreateLoop(const UHeartGraphNode* A, const UHeartGraphNode* B);


	/**			TYPED GETTERS			*/

	// Gets the Heart Graph from an object representing a Heart Graph Node, and attempts to cast it to the requested class.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Graph", ExpandBoolAsExecs = "ReturnValue", DefaultToSelf = "Node"))
	static bool GetGraphTyped(const TScriptInterface<IHeartGraphNodeInterface>& Node, TSubclassOf<UHeartGraph> Class, UHeartGraph*& Graph);

	// Gets the Heart Node from an object representing a Heart Graph Pin, and attempts to cast it to the requested class.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Node", ExpandBoolAsExecs = "ReturnValue"))
	static bool GetGraphNodeTyped(const TScriptInterface<IHeartGraphPinInterface>& Pin, TSubclassOf<UHeartGraphNode> Class, UHeartGraphNode*& Node);

	// Gets the Node Object from an object representing a Heart Graph Node, and attempts to cast it to the requested class.
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = "Class", DynamicOutputParam = "Object", ExpandBoolAsExecs = "ReturnValue", DefaultToSelf = "Node"))
	static bool GetNodeObjectTyped(const TScriptInterface<IHeartGraphNodeInterface>& Node, TSubclassOf<UObject> Class, UObject*& Object);


	/**			STRUCT CONVERTERS		*/

	UFUNCTION(BlueprintPure, Category = "Heart|GraphPin")
	static FHeartGraphPinReference MakeReference(const TScriptInterface<IHeartGraphNodeInterface>& Node, const TScriptInterface<IHeartGraphPinInterface>& Pin);

	// @todo enable UFUNCTION in whenever UE supports TOptional pins
	//UFUNCTION(BlueprintPure, Category = "Heart|GraphPin")
	static TOptional<FHeartGraphPinDesc> ResolvePinDesc(const TScriptInterface<IHeartGraphInterface>& Graph, const FHeartGraphPinReference& Reference);


	// Makes a Node Source from a Class. Add this to a Registry to spawn nodes with instances of this class.
	UFUNCTION(BlueprintPure, Category = "Heart|Registry")
	static FHeartNodeSource MakeNodeSourceFromClass(UClass* Class);

	// Makes a Node Source from an Object. Add this to a Registry to spawn nodes with this object.
	UFUNCTION(BlueprintPure, Category = "Heart|Registry")
	static FHeartNodeSource MakeNodeSourceFromObject(UObject* Object);

	// Converts a Node Source to a class or returns nullptr
	UFUNCTION(BlueprintPure, Category = "Heart|Registry", meta = (DeterminesOutputType = "BaseClass"))
	static const UClass* NodeSourceToClass(const FHeartNodeSource& NodeSource, const UClass* BaseClass);

	// Converts a Node Source to a object or returns nullptr
	UFUNCTION(BlueprintPure, Category = "Heart|Registry", meta = (DeterminesOutputType = "BaseClass"))
	static const UObject* NodeSourceToObject(const FHeartNodeSource& NodeSource, const UClass* BaseClass);
};