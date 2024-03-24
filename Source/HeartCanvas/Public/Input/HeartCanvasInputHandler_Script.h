// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Move_To_UMG/HeartUMGInputBinding.h"
#include "HeartCanvasInputHandler_Script.generated.h"

struct FEventReply;

/**
 *
 */
UCLASS(Blueprintable, Abstract)
class HEARTCANVAS_API UHeartCanvasInputHandler_Script : public UHeartUMGInputBinding_Handler
{
	GENERATED_BODY()

public:
	virtual FReply TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|InputHandlerScript")
	FEventReply HandleEvent(UWidget* Widget, const FHeartInputActivation& Trip) const;
};