// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartInputTypes.h"
#include "HeartWidgetInputBindingBase.h"
#include "HeartWidgetInputBinding_TriggerBase.generated.h"

/**
 *
 */
UCLASS(Abstract)
class HEARTCORE_API UHeartWidgetInputBinding_TriggerBase : public UHeartWidgetInputBindingBase
{
	GENERATED_BODY()

public:
	// Bind triggers to TriggerEvent function. Children should override that, instead of these.
	virtual bool Bind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;
	virtual bool Unbind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;

protected:
	virtual FReply TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Trigger")
	bool HandleInput = true;
};
