// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputHandlerAsset.h"
#include "UI/HeartWidgetInputCondition.h"

FText UHeartWidgetInputHandlerAsset::GetDescription(const UWidget* TestWidget) const
{
	return FText();
}

bool UHeartWidgetInputHandlerAsset::PassCondition(const UWidget* TestWidget) const
{
	if (IsValid(Condition))
	{
		return Condition->TryPassCondition(TestWidget);
	}

	// With no condition, allow any input through.
	return true;
}
