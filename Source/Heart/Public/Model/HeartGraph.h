// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGuids.h"
#include "HeartGraph.generated.h"

class UHeartGraphNode;
class UHeartGraphBehavior;

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
	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	FHeartGraphGuid GetGuid() const { return Guid; }


	/****************************/
	/**		CLASS BEHAVIOR		*/
	/****************************/
public:
	/** Override to specify the behavior class for this graph class */
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|Graph")
	TSubclassOf<UHeartGraphBehavior> GetBehaviorClass() const;

	template <typename THeartGraphBehaviorClass>
	const THeartGraphBehaviorClass* GetBehaviorTyped()
	{
		return Cast<THeartGraphBehaviorClass>(GetBehavior());
	}

	UHeartGraphNode* GetNode(const FHeartNodeGuid& NodeGuid);

	static const UHeartGraphBehavior* GetBehaviorStatic(TSubclassOf<UHeartGraph> HeartGraphClass);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	const UHeartGraphBehavior* GetBehavior() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class), DisplayName = "Get Behavior Typed")
	const UHeartGraphBehavior* GetBehaviorTyped_K2(TSubclassOf<UHeartGraphBehavior> Class) const;


	/****************************/
	/**		NODE EDITING		*/
	/****************************/
public:
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|Graph")
	TArray<UHeartGraphNode*> GetNodeArray() const
	{
		TArray<TObjectPtr<UHeartGraphNode>> NodeArray;
		Nodes.GenerateValueArray(NodeArray);
		return NodeArray;
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	void AddNode(UHeartGraphNode* Node);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	bool RemoveNode(UHeartGraphNode* Node);

private:
	UPROPERTY(SaveGame)
	FHeartGraphGuid Guid;

	// @todo probably dont need to store as MAP? nothing accesses it like one? an array would probably be fine
	UPROPERTY(SaveGame)
	TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>> Nodes;
};