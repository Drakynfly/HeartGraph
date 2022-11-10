// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Components/Widget.h"
#include "HeartWidgetInputCondition.generated.h"

DECLARE_DELEGATE_RetVal_OneParam(
	bool, FHeartWidgetInputCondition, UWidget* /** Widget */);

UCLASS(Abstract, Const, DefaultToInstanced, EditInlineNew, CollapseCategories)
class HEARTCORE_API UHeartWidgetInputCondition : public UObject
{
	GENERATED_BODY()

public:
	virtual FHeartWidgetInputCondition CreateCondition() const { return FHeartWidgetInputCondition(); }
};
