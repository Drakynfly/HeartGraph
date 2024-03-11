// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputHandlerAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartWidgetInputHandlerAsset)

FText UHeartWidgetInputHandlerAsset::GetDescription(const UWidget* TestWidget) const
{
	return FText::GetEmpty();
}

bool UHeartWidgetInputHandlerAsset::PassCondition(const UWidget* TestWidget) const
{
	return true;
}