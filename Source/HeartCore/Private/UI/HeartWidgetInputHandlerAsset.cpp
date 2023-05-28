// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputHandlerAsset.h"

FText UHeartWidgetInputHandlerAsset::GetDescription(const UWidget* TestWidget) const
{
	return FText();
}

bool UHeartWidgetInputHandlerAsset::PassCondition(const UWidget* TestWidget) const
{
	return true;
}
