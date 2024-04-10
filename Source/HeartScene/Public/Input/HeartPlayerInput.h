// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "EnhancedPlayerInput.h"

#include "HeartPlayerInput.generated.h"

/**
 * A simple override of UEnhancedPlayerInput to defer input to an input linker if one is able to be found.
 */
UCLASS()
class HEARTSCENE_API UHeartPlayerInput : public UEnhancedPlayerInput
{
	GENERATED_BODY()

public:
	virtual bool InputKey(const FInputKeyParams& Params) override;
};