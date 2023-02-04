// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Components/Widget.h"
#include "HeartWidgetInputCondition.generated.h"

UCLASS(Abstract, Const, DefaultToInstanced, EditInlineNew, CollapseCategories)
class HEARTCORE_API UHeartWidgetInputCondition : public UObject
{
	GENERATED_BODY()

public:
	virtual bool TryPassCondition(const UWidget* Widget) const { return false; }
};
