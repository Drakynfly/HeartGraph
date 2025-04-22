// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartSceneActor.h"
#include "HeartSceneGenerator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSceneActor)

namespace Heart
{
	const FLazyName SceneGeneratorClass("SceneGenerator");
}

AHeartSceneActor::AHeartSceneActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// @note: This crashes if overriden in BP with a Blueprint component, defeating the purpose entirely. Revert back
	// if/when Epic fixes this.
	//SceneGenerator = CreateDefaultSubobject<UHeartSceneGenerator>(Heart::SceneGeneratorClass);
}

void AHeartSceneActor::BeginPlay()
{
	SceneGenerator = FindComponentByClass<UHeartSceneGenerator>();
	if (!IsValid(SceneGenerator))
	{
		UE_LOG(LogTemp, Warning, TEXT("HeartSceneActor '%s' does not have a Scene Generate component. Please add one!"), *GetName())
	}
	Super::BeginPlay();
}
