// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBinding_TriggerBase.h"
#include "UI/HeartWidgetInputLinker.h"
#include "UI/HeartWidgetInputTrigger.h"

bool UHeartWidgetInputBinding_TriggerBase::Bind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const
{
	if (IsValid(Event))
	{
		auto&& NewEvent = Event->CreateEvent();

		Heart::Input::FConditionalInputCallback InputCallback;
		InputCallback.Callback = NewEvent.Callback;
		InputCallback.Layer = NewEvent.Layer;

		if (Condition)
		{
			InputCallback.Condition = Condition->CreateCondition();
		}

		for (auto&& Trigger : InTriggers)
		{
			if (Trigger.IsValid())
			{
				Linker->BindInputCallback(Trigger.GetMutable<FHeartWidgetInputTrigger>().CreateTrip(), InputCallback);
			}
		}

		return true;
	}

	return false;
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