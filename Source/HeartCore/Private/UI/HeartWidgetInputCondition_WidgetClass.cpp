// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputCondition_WidgetClass.h"

FHeartWidgetInputCondition UHeartWidgetInputCondition_WidgetClass::CreateCondition() const
{
	return FHeartWidgetInputCondition::CreateLambda([=](UWidget* Widget)
	{
		return Widget && Widget->GetClass()->IsChildOf(WidgetClass);
	});
}