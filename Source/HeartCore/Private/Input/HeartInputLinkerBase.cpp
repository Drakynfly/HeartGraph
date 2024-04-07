// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputLinkerBase.h"

#include "Input/HeartInputHandlerAssetBase.h"
#include "Input/HeartInputBindingAsset.h"

#include "HeartCorePrivate.h"
#include "Input/HeartInputActivation.h"
#include "Input/HeartEvent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputLinkerBase)

// @todo hook these back up somewhere
DECLARE_CYCLE_STAT(TEXT("HandleOnMouseWheel"),		STAT_HandleOnMouseWheel, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleOnMouseButtonDown"),	STAT_HandleOnMouseButtonDown, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleOnMouseButtonUp"),	STAT_HandleOnMouseButtonUp, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleOnKeyDown"),			STAT_HandleOnKeyDown, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleOnKeyUp"),			STAT_HandleOnKeyUp, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleOnDragDetected"),	STAT_HandleOnDragDetected, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleNativeOnDragOver"),	STAT_HandleNativeOnDragOver, STATGROUP_HeartCore);
DECLARE_CYCLE_STAT(TEXT("HandleNativeOnDrop"),		STAT_HandleNativeOnDrop, STATGROUP_HeartCore);

DECLARE_CYCLE_STAT(TEXT("HandleManualInput"), STAT_HandleManualInput, STATGROUP_HeartCore);

using namespace Heart::Input;

FCallbackQuery::FCallbackQuery(const UHeartInputLinkerBase* Linker, const FInputTrip& Trip)
{
	Linker->InputCallbackMappings.MultiFindPointer(Trip, Callbacks);
}

FCallbackQuery& FCallbackQuery::Sort()
{
	Algo::Sort(Callbacks,
		[](const TSharedPtr<const FConditionalCallback>* A, const TSharedPtr<const FConditionalCallback>* B)
		{
			return *A->Get() < *B->Get();
		});
	return *this;
}

FCallbackQuery& FCallbackQuery::ForEachWithBreak(const UObject* Target, const TFunctionRef<bool(const FConditionalCallback&)>& Predicate)
{
	for (auto&& CallbackPtr : Callbacks)
	{
		const FConditionalCallback& Ref = *CallbackPtr->Get();

		bool PassedCondition = true;

		if (Ref.Condition.IsBound())
		{
			PassedCondition = Ref.Condition.Execute(Target);
		}

		if (!PassedCondition)
		{
			continue;
		}

		if (!Ref.Handler.IsBound())
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

FHeartEvent UHeartInputLinkerBase::QuickTryCallbacks(const FInputTrip& Trip, UObject* Target, const FHeartInputActivation& Activation)
{
	TOptional<FHeartEvent> Return;

	Query(Trip).ForEachWithBreak(Target,
		[&](const FConditionalCallback& Ref)
		{
			const FHeartEvent Event = Ref.Handler.Execute(Target, Activation);

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

void UHeartInputLinkerBase::BindInputCallback(const FInputTrip& Trip, const TSharedPtr<const FConditionalCallback>& InputCallback)
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
	SCOPE_CYCLE_COUNTER(STAT_HandleNativeOnDrop)

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
		if (ConditionalInputCallback.Key.Type != Manual)
		{
			continue;
		}

		bool PassedCondition = true;

		if (ConditionalInputCallback.Value->Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback.Value->Condition.Execute(Target);
		}

		if (!PassedCondition)
		{
			continue;
		}

		if (ConditionalInputCallback.Value->Description.IsBound())
		{
			Results.Add({ConditionalInputCallback.Key.CustomKey, ConditionalInputCallback.Value->Description.Execute(Target)});
		}
		else
		{
			Results.Add({ConditionalInputCallback.Key.CustomKey});
		}
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

		Binding.InputHandler->Bind(this, Binding.Triggers);
	}
}

void UHeartInputLinkerBase::RemoveBindings(const TArray<FHeartBoundInput>& Bindings)
{
	for (auto&& Binding : Bindings)
	{
		if (!IsValid(Binding.InputHandler)) continue;

		Binding.InputHandler->Unbind(this, Binding.Triggers);
	}
}