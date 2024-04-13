// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputLinkerBase.h"

#include "Input/HeartInputHandlerAssetBase.h"
#include "Input/HeartInputBindingAsset.h"

#include "HeartCorePrivate.h"
#include "Input/HeartInputActivation.h"
#include "Input/HeartEvent.h"
#include "Input/HeartInputTrigger.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputLinkerBase)

DECLARE_CYCLE_STAT(TEXT("QuickTryCallbacks"), STAT_QuickTryCallbacks, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleManualInput"), STAT_HandleManualInput, STATGROUP_HeartCore);

using namespace Heart::Input;

FCallbackQuery::FCallbackQuery(const UHeartInputLinkerBase* Linker, const FHeartInputTrip& Trip)
{
	if (Linker->InputCallbackMappings.Contains(Trip))
	{
		Callbacks = Linker->InputCallbackMappings[Trip].Callbacks;
	} else
	{
		Callbacks.Empty();
	}
}

FCallbackQuery& FCallbackQuery::Sort()
{
	Algo::Sort(Callbacks,
		[](const FHeartSortableInputCallback& A, const FHeartSortableInputCallback& B)
		{
			// Sort in reverse. Higher priorities should be ordered first, lower after.
			return A.Priority > B.Priority;
		});
	return *this;
}

FCallbackQuery& FCallbackQuery::ForEachWithBreak(const UObject* Target, const TFunctionRef<bool(const FHeartSortableInputCallback&)>& Predicate)
{
	for (auto&& CallbackPtr : Callbacks)
	{
		const FHeartSortableInputCallback& Ref = CallbackPtr;

		if (!IsValid(Ref.Handler))
		{
			continue;
		}

		if (!Ref.Handler->PassCondition(Target))
		{
			continue;
		}

		if (!Predicate(Ref))
		{
			break;
		}
	}
	return *this;
}

FHeartEvent UHeartInputLinkerBase::QuickTryCallbacks(const FHeartInputTrip& Trip, UObject* Target, const FHeartInputActivation& Activation)
{
	SCOPE_CYCLE_COUNTER(STAT_QuickTryCallbacks)

	TOptional<FHeartEvent> Return;

	Query(Trip).ForEachWithBreak(Target,
		[&](const FHeartSortableInputCallback& Ref)
		{
			const FHeartEvent Event = Ref.Handler->OnTriggered(Target, Activation);

			// If Priority is Event, this event Reply is allowed to stop capture input, and break out of the input handling loop
			if (Ref.Priority <= EHeartInputExecutionOrder::HighestHandlingPriority)
			{
				if (Event.WasEventCaptured())
				{
					Return = Event;
					return false;
				}
			}

			return true;
		});

	if (Return.IsSet())
	{
		return Return.GetValue();
	}

	// Nothing handled input.
	return FHeartEvent::Ignored;
}

void UHeartInputLinkerBase::BindInputCallback(const FHeartInputTrip& Trip, const FHeartSortableInputCallback& InputCallback)
{
	if (ensure(Trip.IsValid()))
	{
		InputCallbackMappings.FindOrAdd(Trip, {}).Callbacks.Add(InputCallback);
	}
}

void UHeartInputLinkerBase::UnbindInputCallback(const FHeartInputTrip& Trip)
{
	InputCallbackMappings.Remove(Trip);
}

FHeartEvent UHeartInputLinkerBase::HandleManualInput(UObject* Target, const FName Key, const FHeartManualEvent& Activation)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleManualInput)

	if (!IsValid(Target) || Key.IsNone())
	{
		return FHeartEvent::Invalid;
	}

	return QuickTryCallbacks(FHeartInputTrip(Key), Target, Activation);
}

TArray<FHeartManualInputQueryResult> UHeartInputLinkerBase::QueryManualTriggers(const UObject* Target) const
{
	TArray<FHeartManualInputQueryResult> Results;

	for (auto&& [Trip, CallbackList] : InputCallbackMappings)
	{
		for (auto&& Callback : CallbackList.Callbacks)
		{
			const UHeartInputHandlerAssetBase* Handler = Callback.Handler;
			if (!IsValid(Handler))
			{
				continue;
			}

			if (Trip.Type != Manual)
			{
				continue;
			}

			if (!Handler->PassCondition(Target))
			{
				continue;
			}

			Results.Add({Trip.CustomKey, Handler->GetDescription(Target)});
		}
	}

	return Results;
}

FCallbackQuery UHeartInputLinkerBase::Query(const FHeartInputTrip& Trip) const
{
	return FCallbackQuery(this, Trip).Sort();
}

void UHeartInputLinkerBase::AddBindings(const TArray<FHeartBoundInput>& Bindings)
{
	for (auto&& Binding : Bindings)
	{
		if (!IsValid(Binding.InputHandler)) continue;

		const FHeartSortableInputCallback Callback{
			Binding.InputHandler,
			Binding.InputHandler->GetExecutionOrder()};

		for (auto&& Trigger : Binding.Triggers)
		{
			if (!Trigger.IsValid())
			{
				continue;
			}

			auto&& Trips = Trigger.Get<FHeartInputTrigger>().CreateTrips();

			for (auto&& Trip : Trips)
			{
				BindInputCallback(Trip, Callback);
			}
		}
	}
}

void UHeartInputLinkerBase::RemoveBindings(const TArray<FHeartBoundInput>& Bindings)
{
	for (auto&& Binding : Bindings)
	{
		if (!IsValid(Binding.InputHandler)) continue;

		for (auto&& Trigger : Binding.Triggers)
		{
			if (!Trigger.IsValid())
			{
				continue;
			}

			auto&& Trips = Trigger.Get<FHeartInputTrigger>().CreateTrips();

			for (auto&& Trip : Trips)
			{
				UnbindInputCallback(Trip);
			}
		}
	}
}