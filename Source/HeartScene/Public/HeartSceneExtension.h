// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Model/HeartGraphExtension.h"
#include "HeartSceneExtension.generated.h"

class AHeartSceneActor;

/**
 *
 */
UCLASS()
class HEARTSCENE_API UHeartSceneExtension : public UHeartGraphExtension
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	UClass* GetConfigClass() const { return DefaultConfigClass.TryLoadClass<UClass>(); }

	TSubclassOf<AHeartSceneActor> GetSceneClass() const { return DefaultSceneClass; }
#endif

protected:
#if WITH_EDITORONLY_DATA
	// #todo kinda weird place to store this, but it'll do for now
	UPROPERTY(EditAnywhere, Category = "Editor", meta = (MetaClass = "/Script/HeartSceneEditor.PreviewSceneConfig"))
	FSoftClassPath DefaultConfigClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AHeartSceneActor> DefaultSceneClass;
#endif
};
