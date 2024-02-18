// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBinding_TriggerBase.h"
#include "UI/HeartWidgetInputLinker.h"
#include "UI/HeartWidgetInputTrigger.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartWidgetInputBinding_TriggerBase)

bool UHeartWidgetInputBinding_TriggerBase::Bind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const
{
	Heart::Input::FConditionalInputCallback InputCallback;

	InputCallback.Description.BindUObject(this, &ThisClass::GetDescription);
	InputCallback.Condition.BindUObject(this, &ThisClass::PassCondition);
	InputCallback.Callback.BindUObject(this, &ThisClass::TriggerEvent);
	InputCallback.Layer = HandleInput ? Heart::Input::Event : Heart::Input::Listener;

	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			auto&& Trips = Trigger.Get<FHeartWidgetInputTrigger>().CreateTrips();

			for (auto&& Trip : Trips)
			{
				Linker->BindInputCallback(Trip, InputCallback);
			}
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
			auto&& Trips = Trigger.Get<FHeartWidgetInputTrigger>().CreateTrips();

			for (auto&& Trip : Trips)
			{
				Linker->UnbindInputCallback(Trip);
			}
		}
	}

	return true;
}

FReply UHeartWidgetInputBinding_TriggerBase::TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const
{
	return FReply::Unhandled();
}