// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Components/CanvasPanel.h"
#include "HeartGraphCanvasPanel.generated.h"

/**
 *
 */
UCLASS()
class HEARTCANVAS_API UHeartGraphCanvasPanel : public UCanvasPanel
{
	GENERATED_BODY()

public:
	UHeartGraphCanvasPanel();

	/** UWidget */
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
	/** UWidget */
};
