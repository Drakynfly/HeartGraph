// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBindingBase.h"
#include "UI/HeartWidgetInputCondition.h"

FText UHeartWidgetInputBindingBase::GetDescription(const UWidget* TestWidget) const
{
	return FText();
}

bool UHeartWidgetInputBindingBase::PassCondition(const UWidget* TestWidget) const
{
	if (IsValid(Condition))
	{
		return Condition->TryPassCondition(TestWidget);
	}

	// With no condition, allow any input through.
	return true;
}
