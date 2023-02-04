// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputCondition.h"
#include "HeartWidgetInputCondition_WidgetClass.generated.h"

/**
 *
 */
UCLASS(meta = (DisplayName = "Condition - Widget Class"))
class UHeartWidgetInputCondition_WidgetClass : public UHeartWidgetInputCondition
{
	GENERATED_BODY()

public:
	virtual bool TryPassCondition(const UWidget* Widget) const override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = true))
	TSubclassOf<UWidget> WidgetClass;
};