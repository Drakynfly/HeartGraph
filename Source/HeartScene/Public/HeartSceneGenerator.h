// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Model/HeartGraphInterface.h"
#include "Model/HeartGraphNode.h"

#include "HeartSceneGenerator.generated.h"

class UHeartGraph;
class UHeartGraphNode3D;
class UHeartNodeLocationModifierStack;
class UHeartSceneNode;

UCLASS(Blueprintable, ClassGroup = ("Heart"), meta = (BlueprintSpawnableComponent))
class HEARTSCENE_API UHeartSceneGenerator : public UActorComponent, public IHeartGraphInterface3D
{
	GENERATED_BODY()

public:
	UHeartSceneGenerator();

	/** IHeartGraphInterface */
	virtual UHeartGraph* GetHeartGraph() const override;
	/** IHeartGraphInterface */

	/** IHeartGraphInterface3D */
	virtual FVector GetNodeLocation3D(const FHeartNodeGuid& Node) const override;
	virtual void SetNodeLocation3D(const FHeartNodeGuid& Node, const FVector& Location, bool InProgressMove) override;
	/** IHeartGraphInterface3D */

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|SceneGenerator")
	void SetDisplayedGraph(UHeartGraph* NewGraph);

	UFUNCTION(BlueprintCallable, Category = "Heart|SceneGenerator")
	void Regenerate();

	UFUNCTION(BlueprintCallable, Category = "Heart|SceneGenerator")
	UHeartSceneNode* GetSceneNode(const FHeartNodeGuid& NodeGuid) const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|SceneGenerator")
	void OnReset();

	UFUNCTION(BlueprintNativeEvent, Category = "Heart|SceneGenerator")
	void Generate();

	TSubclassOf<UHeartSceneNode> GetVisualClassForNode(const UHeartGraphNode* GraphNode) const;

	UFUNCTION(BlueprintCallable, Category = "Heart|SceneGenerator")
	UHeartSceneNode* AddNodeToDisplay(UHeartGraphNode* GraphNode);

protected:
	UPROPERTY()
	TObjectPtr<UHeartGraph> Graph;

	UPROPERTY()
	TMap<FHeartNodeGuid, TObjectPtr<UHeartSceneNode>> SceneNodes;

	UPROPERTY(VisibleAnywhere, Instanced, Category = "Visualization", NoClear, meta = (ShowInnerProperties))
	TObjectPtr<UHeartNodeLocationModifierStack> LocationModifiers;

	// Should the LocationModifiers work with world space vectors, instead of relative?
	UPROPERTY(EditAnywhere, Category = "Visualization")
	bool UseWorldSpaceInModifiers;
};