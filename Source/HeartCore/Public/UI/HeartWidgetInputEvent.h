// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartInputActivation.h"
#include "Components/Widget.h"
#include "HeartWidgetInputEvent.generated.h"

DECLARE_DELEGATE_RetVal_TwoParams(
	FReply, FHeartWidgetLinkedInput, UWidget* /** Widget */, const struct FHeartInputActivation& /** Activation */);

UCLASS(Abstract, Const, DefaultToInstanced, EditInlineNew, CollapseCategories)
class HEARTCORE_API UHeartWidgetInputEvent : public UObject
{
	GENERATED_BODY()

public:
	virtual FHeartWidgetLinkedInput CreateDelegate() const { return FHeartWidgetLinkedInput(); }
};
