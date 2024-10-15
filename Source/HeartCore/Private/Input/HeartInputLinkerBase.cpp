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

FCallbackQuery::FCallbackQuery(const UHeartInputLinkerBase* Linker, const FInputTrip& Trip)
{
	Linker->InputCallbackMappings.MultiFindPointer(Trip, Callbacks);
}

FCallbackQuery& FCallbackQuery::Sort()
{
	Algo::Sort(Callbacks,
		[](const FSortableCallback* A, const FSortableCallback* B)
		{
			return *A < *B;
		});
	return *this;
}

FCallbackQuery& FCallbackQuery::ForEachWithBreak(const UObject* Target, const TFunctionRef<bool(const FSortableCallback&)>& Predicate)
{
	for (auto&& CallbackPtr : Callbacks)
	{
		const FSortableCallback& Ref = *CallbackPtr;

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

void UHeartInputLinkerBase::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(InThis, Collector);

	const UHeartInputLinkerBase* This = CastChecked<UHeartInputLinkerBase>(InThis);

	for (auto&& Element : This->InputCallbackMappings)
	{
		if (!Element.Value.Handler->IsAsset())
		{
			Collector.AddReferencedObject(const_cast<FSortableCallback*>(&Element.Value)->Handler, This);
		}
	}
}

FHeartEvent UHeartInputLinkerBase::QuickTryCallbacks(const FInputTrip& Trip, UObject* Target, const FHeartInputActivation& Activation)
{
	SCOPE_CYCLE_COUNTER(STAT_QuickTryCallbacks)

	TOptional<FHeartEvent> Return;

	Query(Trip).ForEachWithBreak(Target,
		[&](const FSortableCallback& Ref)
		{
			const FHeartEvent Event = Ref.Handler->OnTriggered(Target, Activation);

			// If Priority is Event, this event Reply is allowed to stop capture input, and break out of the input handling loop
			if (Ref.Priority <= HighestHandlingPriority)
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

void UHeartInputLinkerBase::BindInputCallback(const FInputTrip& Trip, const FSortableCallback& InputCallback)
{
	if (ensure(Trip.IsValid()))
	{
		InputCallbackMappings.Add(Trip, InputCallback);
	}
}

void UHeartInputLinkerBase::UnbindInputCallback(const FInputTrip& Trip)
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

	return QuickTryCallbacks(FInputTrip(Key), Target, Activation);
}

TArray<FHeartManualInputQueryResult> UHeartInputLinkerBase::QueryManualTriggers(const UObject* Target) const
{
	TArray<FHeartManualInputQueryResult> Results;

	for (auto&& ConditionalInputCallback : InputCallbackMappings)
	{
		const UHeartInputHandlerAssetBase* Handler = ConditionalInputCallback.Value.Handler;
		if (!IsValid(Handler))
		{
			continue;
		}

		if (ConditionalInputCallback.Key.Type != Manual)
		{
			continue;
		}

		if (!Handler->PassCondition(Target))
		{
			continue;
		}

		Results.Add({ConditionalInputCallback.Key.CustomKey, Handler->GetDescription(Target)});
	}

	return Results;
}

FCallbackQuery UHeartInputLinkerBase::Query(const FInputTrip& Trip) const
{
	return FCallbackQuery(this, Trip).Sort();
}

void UHeartInputLinkerBase::AddBindings(const TArray<FHeartBoundInput>& Bindings)
{
	for (auto&& Binding : Bindings)
	{
		if (!IsValid(Binding.InputHandler)) continue;

		const FSortableCallback Callback{
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