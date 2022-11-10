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
	/**		GUID				*/
	/****************************/
public:
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	FHeartGraphGuid GetGuid() const { return Guid; }


	/****************************/
	/**		CLASS BEHAVIOR		*/
	/****************************/
public:
	/** Override to specify the behavior class for this graph class */
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|Graph")
	TSubclassOf<UHeartGraphSchema> GetSchemaClass() const;

	template <typename THeartGraphSchemaClass>
	const THeartGraphSchemaClass* GetSchemaTyped()
	{
		return Cast<THeartGraphSchemaClass>(GetSchema());
	}

	static const UHeartGraphSchema* GetSchemaStatic(TSubclassOf<UHeartGraph> HeartGraphClass);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	const UHeartGraphSchema* GetSchema() const;

	UE_DEPRECATED(5.0, TEXT("Use GetSchema instead"))
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	const UHeartGraphSchema* GetBehavior() const { return GetSchema(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class), DisplayName = "Get Schema Typed")
	const UHeartGraphSchema* GetSchemaTyped_K2(TSubclassOf<UHeartGraphSchema> Class) const;

	UE_DEPRECATED(5.0, TEXT("Use GetSchemaTyped_K2 instead"))
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class), DisplayName = "Get Behavior Typed")
	const UHeartGraphSchema* GetBehaviorTyped_K2(TSubclassOf<UHeartGraphSchema> Class) const { return GetSchemaTyped_K2(Class); }


	/****************************/
	/**		NODE EDITING		*/
	/****************************/
public:
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

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	void AddNode(UHeartGraphNode* Node);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	bool RemoveNode(UHeartGraphNode* Node);


public:
	UPROPERTY(BlueprintAssignable, Category = "Heart|Graph|Events")
	FHeartGraphNodeEvent OnNodeAdded;

	UPROPERTY(BlueprintAssignable, Category = "Heart|Graph|Events")
	FHeartGraphNodeEvent OnNodeRemoved;

private:
	UPROPERTY()
	FHeartGraphGuid Guid;

	// @todo probably dont need to store as MAP? nothing accesses it like one? an array would probably be fine
	UPROPERTY()
	TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>> Nodes;
};