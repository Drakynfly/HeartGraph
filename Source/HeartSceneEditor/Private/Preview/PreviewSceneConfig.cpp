// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Preview/PreviewSceneConfig.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PreviewSceneConfig)

UPreviewSceneConfig::UPreviewSceneConfig()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPreviewSceneConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	OnConfigEdit.ExecuteIfBound(PropertyChangedEvent);
}