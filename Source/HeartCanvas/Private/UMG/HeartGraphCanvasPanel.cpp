// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UMG/HeartGraphCanvasPanel.h"
#include "HeartCanvasPaletteCategory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphCanvasPanel)

UHeartGraphCanvasPanel::UHeartGraphCanvasPanel()
{
	// Canvas graph panels should always intercept input
	SetVisibilityInternal(ESlateVisibility::Visible);

	// Canvas nodes should be clipped to the canvas
	SetClipping(EWidgetClipping::ClipToBoundsAlways);
}

#if WITH_EDITOR
const FText UHeartGraphCanvasPanel::GetPaletteCategory()
{
	return Heart::Canvas::PaletteCategory::Default;
}
#endif