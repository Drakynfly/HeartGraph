// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Move_To_UMG/HeartCanvasInputBinding.h"
#include "HeartCanvasInputHandler_Script.generated.h"

struct FEventReply;

/**
 *
 */
UCLASS(Blueprintable, Abstract)
class HEARTCANVAS_API UHeartCanvasInputHandler_Script : public UHeartCanvasInputHandlerBase
{
	GENERATED_BODY()

public:
	virtual FReply OnTriggered(UWidget* Widget, const FHeartInputActivation& Trip) const override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|InputHandlerScript")
	FEventReply HandleEvent(UWidget* Widget, const FHeartInputActivation& Trip) const;
};