// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputCondition_WidgetClass.h"

bool UHeartWidgetInputCondition_WidgetClass::TryPassCondition(const UWidget* Widget) const
{
	return IsValid(Widget) && Widget->GetClass()->IsChildOf(WidgetClass);
}
