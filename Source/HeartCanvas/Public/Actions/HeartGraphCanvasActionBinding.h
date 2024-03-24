// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Move_To_UMG/HeartUMGInputBinding.h"
#include "HeartGraphCanvasActionBinding.generated.h"

class UHeartGraphCanvasAction;

UCLASS(meta = (DisplayName = "Event - Canvas Action"))
class HEARTCANVAS_API UHeartGraphCanvasActionBinding : public UHeartUMGInputBinding_Handler
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(const UWidget* TestWidget) const override;
	virtual bool PassCondition(const UWidget* TestWidget) const override;
	virtual FReply TriggerEvent(UWidget* Widget, const FHeartInputActivation& Activation) const override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartGraphCanvasAction> ActionClass;
};