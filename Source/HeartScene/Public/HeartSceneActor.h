// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "HeartSceneActor.generated.h"

class UHeartSceneGenerator;

namespace Heart
{
	static const FLazyName SceneGeneratorClass("SceneGenerator");
}

UCLASS(Abstract)
class HEARTSCENE_API AHeartSceneActor : public AActor
{
	GENERATED_BODY()

public:
	AHeartSceneActor();

	UHeartSceneGenerator* GetGenerator() const { return SceneGenerator; }

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Heart")
	TObjectPtr<UHeartSceneGenerator> SceneGenerator;
};