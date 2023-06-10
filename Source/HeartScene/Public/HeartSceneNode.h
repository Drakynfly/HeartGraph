// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"

#include "HeartSceneNode.generated.h"

class UHeartGraphNode;
class UHeartSceneGenerator;

/**
 * Base class for a 3D representation of a Heart node in a graph visualizer.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = ("Heart"))
class HEARTSCENE_API UHeartSceneNode : public USceneComponent
{
	GENERATED_BODY()

	friend UHeartSceneGenerator;

public:
	UHeartSceneNode();

protected:
	UPROPERTY()
	TWeakObjectPtr<UHeartGraphNode> GraphNode;

	UPROPERTY()
	TWeakObjectPtr<UHeartSceneGenerator> Generator;
};
