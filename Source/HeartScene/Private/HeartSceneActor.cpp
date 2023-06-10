// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartSceneActor.h"

#include "HeartSceneGenerator.h"

AHeartSceneActor::AHeartSceneActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneGenerator = CreateDefaultSubobject<UHeartSceneGenerator>(Heart::SceneGeneratorClass);
}
