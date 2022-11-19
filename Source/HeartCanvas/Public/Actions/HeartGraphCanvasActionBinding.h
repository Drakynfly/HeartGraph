// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UI/HeartWidgetInputEvent.h"
#include "HeartGraphCanvasActionBinding.generated.h"

class UHeartGraphCanvasAction;

UCLASS(meta = (DisplayName = "Canvas Graph Action Event"))
class HEARTCANVAS_API UHeartGraphCanvasActionBinding : public UHeartWidgetInputEvent
{
	GENERATED_BODY()

public:
	virtual FHeartWidgetLinkedEvent CreateEvent() const override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartGraphCanvasAction> ActionClass;
};

UCLASS(meta = (DisplayName = "Canvas Graph Action Listener"))
class HEARTCANVAS_API UHeartGraphCanvasActionListener : public UHeartWidgetInputListener
{
	GENERATED_BODY()

public:
	virtual FHeartWidgetLinkedListener CreateListener() const override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartGraphCanvasAction> ActionClass;
};