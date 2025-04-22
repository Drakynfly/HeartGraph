// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "HeartSceneActor.generated.h"

class UHeartSceneGenerator;

namespace Heart
{
	static const FLazyName SceneGeneratorClass("SceneGenerator");
}

/*
 * @todo In 5.5 overriding an actor component with a Blueprint component crashes on level load, due to creating a subobject on a async loading thread.
 * Workaround is to require the blueprint child to add a component themselves.
 * Crash only occurs with Game Target.
 */
UCLASS(Abstract)
class HEARTSCENE_API AHeartSceneActor : public AActor
{
	GENERATED_BODY()

public:
	AHeartSceneActor();

	virtual void BeginPlay() override;

	UHeartSceneGenerator* GetGenerator() const { return SceneGenerator; }

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Heart")
	TObjectPtr<UHeartSceneGenerator> SceneGenerator;
};