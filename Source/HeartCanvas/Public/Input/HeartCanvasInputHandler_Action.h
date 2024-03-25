// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Move_To_UMG/HeartCanvasInputBinding.h"
#include "HeartCanvasInputHandler_Action.generated.h"

class UHeartGraphActionBase;

UCLASS(meta = (DisplayName = "Graph Action"))
class HEARTCANVAS_API UHeartCanvasInputHandler_Action : public UHeartCanvasInputHandlerBase
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(const UWidget* TestWidget) const override;
	virtual bool PassCondition(const UWidget* TestWidget) const override;
	virtual FReply OnTriggered(UWidget* Widget, const FHeartInputActivation& Activation) const override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartGraphActionBase> ActionClass;
};