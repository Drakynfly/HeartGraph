// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UI/HeartWidgetInputBinding_TriggerBase.h"
#include "HeartGraphCanvasActionBinding.generated.h"

class UHeartGraphCanvasAction;

UCLASS(meta = (DisplayName = "Event - Canvas Action"))
class HEARTCANVAS_API UHeartGraphCanvasActionBinding : public UHeartWidgetInputBinding_TriggerBase
{
	GENERATED_BODY()

public:
	virtual FReply TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartGraphCanvasAction> ActionClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	bool CaptureMouse;
};