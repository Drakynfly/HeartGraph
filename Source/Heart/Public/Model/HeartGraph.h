// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGuids.h"
#include "HeartGraph.generated.h"

class UHeartGraphNode;
class UHeartGraphSchema;

DECLARE_LOG_CATEGORY_EXTERN(LogHeartGraph, Log, All)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHeartGraphEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeartGraphNodeEvent, UHeartGraphNode*, Node);

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class HEART_API UHeartGraph : public UObject
{
	GENERATED_BODY()


	/****************************/
	/**		GETTERS				*/
	/****************************/
public:
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	FHeartGraphGuid GetGuid() const { return Guid; }

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	UHeartGraphNode* GetNode(const FHeartNodeGuid& NodeGuid) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|Graph")
	void GetNodeArray(TArray<UHeartGraphNode*>& OutNodes) const
	{
		// *le sign* epic templates mess this up . . .
		TArray<TObjectPtr<UHeartGraphNode>> NodeArray;
		Nodes.GenerateValueArray(NodeArray);
		OutNodes = NodeArray;
	}


	/****************************/
	/**		CLASS BEHAVIOR		*/
	/****************************/
public:
	/** Override to specify the behavior class for this graph class */
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|Graph")
	TSubclassOf<UHeartGraphSchema> GetSchemaClass() const;

	template <typename THeartGraphSchema>
	const THeartGraphSchema* GetSchemaTyped() const
	{
		static_assert(TIsDerivedFrom<THeartGraphSchema, UHeartGraphSchema>::IsDerived, "The schema class must derive from UHeartGraphSchema");
		return Cast<THeartGraphSchema>(GetSchema());
	}

	static const UHeartGraphSchema* GetSchemaStatic(TSubclassOf<UHeartGraph> HeartGraphClass);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	const UHeartGraphSchema* GetSchema() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class), DisplayName = "Get Schema Typed")
	const UHeartGraphSchema* GetSchemaTyped_K2(TSubclassOf<UHeartGraphSchema> Class) const;


	/****************************/
	/**		NODE EDITING		*/
	/****************************/
public:
	template <typename THeartGraphNode>
	THeartGraphNode* CreateNode(const FVector2D& Location)
	{
		static_assert(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::IsDerived, "The node class must derive from UHeartGraphNode");
		return Cast<THeartGraphNode>(CreateNode(THeartGraphNode::StaticClass(), Location));
	}

	template <typename THeartGraphNode>
	THeartGraphNode* CreateNode(const TSubclassOf<UHeartGraphNode> Class, const FVector2D& Location)
	{
		static_assert(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::IsDerived, "The node class must derive from UHeartGraphNode");
		check(Class->IsChildOf<THeartGraphNode>());
		return Cast<THeartGraphNode>(CreateNode(Class, Location));
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode", meta = (DeterminesOutputType = Class))
	UHeartGraphNode* CreateNode(TSubclassOf<UHeartGraphNode> Class, const FVector2D& Location);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	void AddNode(UHeartGraphNode* Node);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	bool RemoveNode(UHeartGraphNode* Node);


public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FHeartGraphNodeEvent OnNodeAdded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FHeartGraphNodeEvent OnNodeRemoved;

private:
	UPROPERTY()
	FHeartGraphGuid Guid;

	// @todo probably dont need to store as MAP? nothing accesses it like one? an array would probably be fine
	UPROPERTY()
	TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>> Nodes;
};