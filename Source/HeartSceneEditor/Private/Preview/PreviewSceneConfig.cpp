// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Preview/PreviewSceneConfig.h"

UPreviewSceneConfig::UPreviewSceneConfig()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPreviewSceneConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	OnConfigEdit.ExecuteIfBound(PropertyChangedEvent);
}
