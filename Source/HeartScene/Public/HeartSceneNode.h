// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "View/HeartVisualizerInterfaces.h"

#include "HeartSceneNode.generated.h"

class UHeartGraphNode;
class UHeartSceneGenerator;

/**
 * Base class for a 3D representation of a Heart node in a graph visualizer.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = ("Heart"))
class HEARTSCENE_API UHeartSceneNode : public USceneComponent, public IGraphNodeVisualizerInterface
{
	GENERATED_BODY()

	friend UHeartSceneGenerator;

public:
	UHeartSceneNode();

	/** IHeartGraphNodeInterface */
	virtual UHeartGraphNode* GetHeartGraphNode() const override;
	/** IHeartGraphNodeInterface */

protected:
	virtual void NativeOnCreated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|SceneNode")
	void OnCreated();

protected:
	UPROPERTY()
	TWeakObjectPtr<UHeartGraphNode> GraphNode;

	UPROPERTY()
	TWeakObjectPtr<UHeartSceneGenerator> Generator;
};
