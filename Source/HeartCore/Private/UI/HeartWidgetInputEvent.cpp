// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputEvent.h"

FHeartWidgetLinkedEvent UHeartWidgetInputListener::CreateEvent() const
{
	auto&& Listener = CreateListener();

	return {FHeartWidgetLinkedEventCallback::CreateLambda([this, Listener](UWidget* Widget, const FHeartInputActivation& Activation)
	{
		Listener.Callback.Execute(Widget, Activation);
		return FReply::Unhandled();
	}), Heart::Input::Listener };
}
