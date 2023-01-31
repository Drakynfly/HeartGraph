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
	virtual bool Bind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const override;
	virtual bool Unbind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const override;

protected:
	virtual FReply TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const;

protected:
	UPROPERTY(EditAnywhere)
	bool HandleInput = true;
};
