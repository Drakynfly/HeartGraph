// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputBindingBase.h"
#include "HeartWidgetInputEvent.h"
#include "HeartWidgetInputBinding_TriggerBase.generated.h"

/**
 *
 */
UCLASS(meta = (DisplayName = "Triggered Event"))
class HEARTCORE_API UHeartWidgetInputBinding_TriggerBase : public UHeartWidgetInputBindingBase
{
	GENERATED_BODY()

public:
	virtual bool Bind(UHeartWidgetInputLinker* Linker) override;
	virtual bool Unbind(UHeartWidgetInputLinker* Linker) override;

protected:
	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<UHeartWidgetInputEvent> Event;
};