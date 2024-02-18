// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "PreviewSceneConfig.generated.h"

class AHeartSceneActor;

UCLASS()
class HEARTSCENEEDITOR_API UPreviewSceneConfig : public UActorComponent
{
	GENERATED_BODY()

public:
	UPreviewSceneConfig();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	using FOnConfigEdit = TDelegate<void(const FPropertyChangedEvent&)>;
	FOnConfigEdit OnConfigEdit;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSoftClassPtr<AHeartSceneActor> SceneClassOverride;
};