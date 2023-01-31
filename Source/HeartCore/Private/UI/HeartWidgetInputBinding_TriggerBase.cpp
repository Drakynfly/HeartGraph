// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBinding_TriggerBase.h"
#include "UI/HeartWidgetInputLinker.h"
#include "UI/HeartWidgetInputTrigger.h"

bool UHeartWidgetInputBinding_TriggerBase::Bind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const
{
	Heart::Input::FConditionalInputCallback InputCallback;

	if (Condition)
	{
		InputCallback.Condition = Condition->CreateCondition();
	}

	InputCallback.Callback = FHeartWidgetLinkedEventCallback::CreateUObject(this, &ThisClass::TriggerEvent);
	InputCallback.Layer = HandleInput ? Heart::Input::Event : Heart::Input::Listener;

	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			Linker->BindInputCallback(Trigger.GetMutable<FHeartWidgetInputTrigger>().CreateTrip(), InputCallback);
		}
	}

	return true;
}

bool UHeartWidgetInputBinding_TriggerBase::Unbind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const
{
	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			Linker->UnbindInputCallback(Trigger.GetMutable<FHeartWidgetInputTrigger>().CreateTrip());
		}
	}

	return true;
}

FReply UHeartWidgetInputBinding_TriggerBase::TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const
{
	return FReply::Unhandled();
}
