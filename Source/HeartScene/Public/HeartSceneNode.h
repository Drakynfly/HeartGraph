// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Components/SceneComponent.h"
#include "View/HeartVisualizerInterfaces.h"

#include "HeartSceneNode.generated.h"

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
	virtual UHeartGraph* GetHeartGraph() const override;
	virtual FHeartNodeGuid GetNodeGuid() const override;
	/** IHeartGraphNodeInterface */

protected:
	virtual void NativeOnCreated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|SceneNode")
	void OnCreated();

protected:
	UPROPERTY()
	FHeartNodeGuid GraphNode;

	UPROPERTY()
	TWeakObjectPtr<UHeartSceneGenerator> Generator;
};
