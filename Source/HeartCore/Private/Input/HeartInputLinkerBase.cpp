// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputLinkerBase.h"

#include "Input/HeartInputHandlerAssetBase.h"
#include "Input/HeartInputBindingAsset.h"

#include "HeartCorePrivate.h"
#include "Input/HeartInputActivation.h"

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

TOptional<FReply> UHeartInputLinkerBase::TryCallbacks(const FInputTrip& Trip, UObject* Target, const FHeartInputActivation& Activation)
{
	TArray<const TSharedPtr<const FConditionalCallback>*> Callbacks;
	InputCallbackMappings.MultiFindPointer(Trip, Callbacks);
	Algo::Sort(Callbacks,
		[](const TSharedPtr<const FConditionalCallback>* A, const TSharedPtr<const FConditionalCallback>* B)
		{
			return *A->Get() < *B->Get();
		});

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

		if (Ref.Handler.IsBound())
		{
			FReply Reply = Ref.Handler.Execute(Target, Activation);

			if (Ref.Layer == Event)
			{
				if (Reply.IsEventHandled())
				{
					return Reply;
				}
			}
		}
	}

	return {};
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

bool UHeartInputLinkerBase::HandleManualInput(UObject* Target, const FName Key, const FHeartManualEvent& Activation)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleNativeOnDrop)

	if (auto Result = TryCallbacks(FInputTrip(Key), Target, Activation);
		Result.IsSet())
	{
		return Result.GetValue().IsEventHandled();
	}

	return false;
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