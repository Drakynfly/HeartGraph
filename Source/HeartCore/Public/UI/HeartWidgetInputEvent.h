// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartInputActivation.h"
#include "HeartInputTypes.h"
#include "Components/Widget.h"
#include "HeartWidgetInputEvent.generated.h"

struct FHeartWidgetLinkedEvent
{
	FHeartWidgetLinkedEventCallback Callback;
	Heart::Input::EHeartInputLayer Layer;
};

UCLASS(Abstract, Const, DefaultToInstanced, EditInlineNew, CollapseCategories)
class HEARTCORE_API UHeartWidgetInputEvent : public UObject
{
	GENERATED_BODY()

public:
	virtual FHeartWidgetLinkedEvent CreateEvent() const { return FHeartWidgetLinkedEvent(); }
};

DECLARE_DELEGATE_TwoParams(
	FHeartWidgetLinkedListenerCallback, UWidget* /** Widget */, const struct FHeartInputActivation& /** Activation */);

struct FHeartWidgetLinkedListener
{
	FHeartWidgetLinkedListenerCallback Callback;
};

UCLASS(Abstract, Const, DefaultToInstanced, EditInlineNew, CollapseCategories)
class HEARTCORE_API UHeartWidgetInputListener : public UHeartWidgetInputEvent
{
	GENERATED_BODY()

private:
	virtual FHeartWidgetLinkedEvent CreateEvent() const override final;

protected:
	virtual FHeartWidgetLinkedListener CreateListener() const { return FHeartWidgetLinkedListener(); }
};
