// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartSceneActor.h"
#include "HeartSceneGenerator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSceneActor)

AHeartSceneActor::AHeartSceneActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneGenerator = CreateDefaultSubobject<UHeartSceneGenerator>(Heart::SceneGeneratorClass);
}