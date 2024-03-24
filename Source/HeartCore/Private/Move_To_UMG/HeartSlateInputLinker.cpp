// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Move_To_UMG/HeartSlateInputLinker.h"

#include "HeartCorePrivate.h"

#include "Input/HeartInputActivation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSlateInputLinker)

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

TOptional<FReply> UHeartSlateInputLinker::TryCallbacks(const FInputTrip& Trip, const TSharedRef<SWidget>& Widget,
	const FHeartInputActivation& Activation)
{
	TArray<const FConditionalCallback*> Callbacks;
	InputCallbackMappings.MultiFindPointer(Trip, Callbacks);
	Callbacks.Sort();

	for (const FConditionalCallback* CallbackPtr : Callbacks)
	{
		const FConditionalCallback& Ref = *CallbackPtr;

		bool PassedCondition = true;

		const auto ConditionCallback = static_cast<TLinkerType<SWidget>::FConditionDelegate*>(Ref.Condition.Get());

		if (ConditionCallback->IsBound())
		{
			PassedCondition = ConditionCallback->Execute(Widget);
		}

		if (!PassedCondition)
		{
			continue;
		}

		const auto HandlerCallback = static_cast<TLinkerType<SWidget>::FHandlerDelegate*>(Ref.Handler.Get());

		if (HandlerCallback->IsBound())
		{
			FReply Reply = HandlerCallback->Execute(Widget, Activation);

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

FReply UHeartSlateInputLinker::HandleOnMouseWheel(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FPointerEvent& PointerEvent)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseWheel)

	FInputTrip MouseWheelAxisTrip;
	MouseWheelAxisTrip.Key = EKeys::MouseWheelAxis;
	MouseWheelAxisTrip.ModifierMask = EModifierKey::FromBools(PointerEvent.IsControlDown(), PointerEvent.IsAltDown(), PointerEvent.IsShiftDown(), PointerEvent.IsCommandDown());
	MouseWheelAxisTrip.Type = Press; // Mouse wheel events must always use the 'Press' type

	// @todo what the heck went on here? does PointerEvent have a different/null EffectingButton?
	const FHeartInputActivation Activation = FPointerEvent(
		PointerEvent.GetUserIndex(),
		PointerEvent.GetPointerIndex(),
		PointerEvent.GetScreenSpacePosition(),
		PointerEvent.GetLastScreenSpacePosition(),
		PointerEvent.GetPressedButtons(),
		EKeys::MouseWheelAxis,
		PointerEvent.GetWheelDelta(),
		PointerEvent.GetModifierKeys());

	if (auto Result = TryCallbacks(MouseWheelAxisTrip, Widget, Activation);
		Result.IsSet())
	{
		return Result.GetValue();
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnMouseButtonDown(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FPointerEvent& PointerEvent)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseButtonDown)

	FInputTrip Trip;
	Trip.Key = PointerEvent.GetEffectingButton().IsValid() ? PointerEvent.GetEffectingButton() : *PointerEvent.GetPressedButtons().CreateConstIterator();
	Trip.ModifierMask = EModifierKey::FromBools(PointerEvent.IsControlDown(), PointerEvent.IsAltDown(), PointerEvent.IsShiftDown(), PointerEvent.IsCommandDown());
	Trip.Type = Press;

	if (auto Result = TryCallbacks(Trip, Widget, PointerEvent);
		Result.IsSet())
	{
		return Result.GetValue();
	}

	// If no regular handles triggered, try DDO triggers.

	/*
	TArray<const FConditionalCallback_DDO*> DropDropTriggerArray;
	DragDropTriggers.MultiFindPointer(Trip, DropDropTriggerArray);
	DropDropTriggerArray.Sort();
	for (const FConditionalCallback_DDO*& CallbackPtr : DropDropTriggerArray)
	{
		const FConditionalCallback_DDO& Ref = *CallbackPtr;

		bool PassedCondition = true;

		if (const auto ConditionCallback = static_cast<TLinkerType<UWidget>::FConditionDelegate*>(Ref.Condition.Get());
			ConditionCallback->IsBound())
		{
			PassedCondition = ConditionCallback->Execute(Widget);
		}

		if (!PassedCondition)
		{
			continue;
		}

		const auto HandlerCallback = static_cast<TLinkerType<UWidget>::FCreateDDODelegate*>(Ref.Handler.Get());
		if (!HandlerCallback->IsBound())
		{
			continue;
		}

		const TSharedPtr<SWidget> SlateWidgetDetectingDrag = Widget->GetCachedWidget();
		if (SlateWidgetDetectingDrag.IsValid())
		{
			FReply Reply = FReply::Handled().DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), PointerEvent.GetEffectingButton());

			if (Ref.Layer == Event)
			{
				if (Reply.IsEventHandled())
				{
					return Reply;
				}
			}
		}
	}
	*/

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnMouseButtonUp(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FPointerEvent& PointerEvent)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseButtonUp)

	FInputTrip Trip;
	Trip.Key = PointerEvent.GetEffectingButton().IsValid() ? PointerEvent.GetEffectingButton() : *PointerEvent.GetPressedButtons().CreateConstIterator();
	Trip.ModifierMask = EModifierKey::FromBools(PointerEvent.IsControlDown(), PointerEvent.IsAltDown(), PointerEvent.IsShiftDown(), PointerEvent.IsCommandDown());
	Trip.Type = Release;

	if (auto Result = TryCallbacks(Trip, Widget, PointerEvent);
		Result.IsSet())
	{
		return Result.GetValue();
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnKeyDown(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FKeyEvent& KeyEvent)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleOnKeyDown)

	FInputTrip Trip;
	Trip.Key = KeyEvent.GetKey();
	Trip.ModifierMask = EModifierKey::FromBools(KeyEvent.IsControlDown(), KeyEvent.IsAltDown(), KeyEvent.IsShiftDown(), KeyEvent.IsCommandDown());
	Trip.Type = Press;

	if (auto Result = TryCallbacks(Trip, Widget, KeyEvent);
		Result.IsSet())
	{
		return Result.GetValue();
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnKeyUp(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FKeyEvent& KeyEvent)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleOnKeyUp)

	FInputTrip Trip;
	Trip.Key = KeyEvent.GetKey();
	Trip.ModifierMask = EModifierKey::FromBools(KeyEvent.IsControlDown(), KeyEvent.IsAltDown(), KeyEvent.IsShiftDown(), KeyEvent.IsCommandDown());
	Trip.Type = Release;

	if (auto Result = TryCallbacks(Trip, Widget, KeyEvent);
		Result.IsSet())
	{
		return Result.GetValue();
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleManualInput(const TSharedRef<SWidget>& Widget, const FName Key,
	const FHeartManualEvent& Activation)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleNativeOnDrop)

	FInputTrip Trip;
	Trip.Type = Manual;
	Trip.CustomKey = Key;

	if (auto Result = TryCallbacks(Trip, Widget, Activation);
		Result.IsSet())
	{
		return Result.GetValue();
	}

	return FReply::Unhandled();
}

TArray<FHeartManualInputQueryResult> UHeartSlateInputLinker::QueryManualTriggers(
	const TSharedRef<SWidget>& Widget) const
{
	TArray<FHeartManualInputQueryResult> Results;

	for (auto&& ConditionalInputCallback : InputCallbackMappings)
	{
		if (ConditionalInputCallback.Key.Type != Manual)
		{
			continue;
		}

		bool PassedCondition = true;

		if (const auto ConditionCallback = static_cast<TLinkerType<SWidget>::FConditionDelegate*>(ConditionalInputCallback.Value.Condition.Get());
			ConditionCallback->IsBound())
		{
			PassedCondition = ConditionCallback->Execute(Widget);
		}

		if (!PassedCondition)
		{
			continue;
		}

		if (const auto DescriptionCallback = static_cast<TLinkerType<SWidget>::FDescriptionDelegate*>(ConditionalInputCallback.Value.Description.Get());
			DescriptionCallback->IsBound())
		{
			Results.Add({ConditionalInputCallback.Key.CustomKey, DescriptionCallback->Execute(Widget)});
		}
		else
		{
			Results.Add({ConditionalInputCallback.Key.CustomKey});
		}
	}

	return Results;
}