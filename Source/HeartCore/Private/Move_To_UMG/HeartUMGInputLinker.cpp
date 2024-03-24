// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Move_To_UMG/HeartUMGInputLinker.h"
#include "Move_To_UMG/HeartDragDropOperation.h"

#include "Input/HeartInputActivation.h"
#include "Input/HeartInputTypes.h"

#include "UI/HeartUMGContextObject.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartUMGInputLinker)

using namespace Heart::Input;

TOptional<FReply> UHeartWidgetInputLinker::TryCallbacks(const FInputTrip& Trip, UWidget* Widget, const FHeartInputActivation& Activation)
{
	TArray<const FConditionalCallback*> Callbacks;
	InputCallbackMappings.MultiFindPointer(Trip, Callbacks);
	Callbacks.Sort();

	for (const FConditionalCallback* CallbackPtr : Callbacks)
	{
		const FConditionalCallback& Ref = *CallbackPtr;

		bool PassedCondition = true;

		const auto ConditionCallback = static_cast<TLinkerType<UWidget>::FConditionDelegate*>(Ref.Condition.Get());

		if (ConditionCallback->IsBound())
		{
			PassedCondition = ConditionCallback->Execute(Widget);
		}

		if (!PassedCondition)
		{
			continue;
		}

		const auto HandlerCallback = static_cast<TLinkerType<UWidget>::FHandlerDelegate*>(Ref.Handler.Get());

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

FReply UHeartWidgetInputLinker::HandleOnMouseWheel(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseWheel)

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

FReply UHeartWidgetInputLinker::HandleOnMouseButtonDown(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseButtonDown)

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

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnMouseButtonUp(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseButtonUp)

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

FReply UHeartWidgetInputLinker::HandleOnKeyDown(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnKeyDown)

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

FReply UHeartWidgetInputLinker::HandleOnKeyUp(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnKeyUp)

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

UHeartDragDropOperation* UHeartWidgetInputLinker::HandleOnDragDetected(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnDragDetected)

	FInputTrip Trip;
	Trip.Key = PointerEvent.GetEffectingButton().IsValid() ? PointerEvent.GetEffectingButton() : *PointerEvent.GetPressedButtons().CreateConstIterator();
	Trip.ModifierMask = EModifierKey::FromBools(PointerEvent.IsControlDown(), PointerEvent.IsAltDown(), PointerEvent.IsShiftDown(), PointerEvent.IsCommandDown());
	Trip.Type = Press;

	TArray<const FConditionalCallback_DDO*> DropDropTriggerArray;
	DragDropTriggers.MultiFindPointer(Trip, DropDropTriggerArray);
	DropDropTriggerArray.Sort();
	for (const FConditionalCallback_DDO*& CallbackPtr : DropDropTriggerArray)
	{
		const FConditionalCallback_DDO& Ref = *CallbackPtr;

		bool PassedCondition = true;

		const auto ConditionCallback = static_cast<TLinkerType<UWidget>::FConditionDelegate*>(Ref.Condition.Get());

		if (ConditionCallback->IsBound())
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

		UHeartDragDropOperation* DragDropOperation = HandlerCallback->Execute(Widget);

		if (!IsValid(DragDropOperation)) continue;

		DragDropOperation->SummonedBy = Widget;

		if (Widget->Implements<UHeartUMGContextObject>())
		{
			DragDropOperation->Payload = IHeartUMGContextObject::Execute_GetContextObject(Widget);
		}

		if (DragDropOperation->SetupDragDropOperation())
		{
			return DragDropOperation;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Created DDO (%s) unnecessarily, figure out why"), *DragDropOperation->GetClass()->GetName())
		}
	}

	return nullptr;
}

bool UHeartWidgetInputLinker::HandleNativeOnDragOver(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleNativeOnDragOver)

	if (auto&& HeartDDO = Cast<UHeartDragDropOperation>(InOperation))
	{
		return HeartDDO->OnHoverWidget(Widget);
	}

	return false;
}

bool UHeartWidgetInputLinker::HandleNativeOnDrop(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent,
												 UDragDropOperation* InOperation)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleNativeOnDrop)

	if (auto&& HeartDDO = Cast<UHeartDragDropOperation>(InOperation))
	{
		return HeartDDO->CanDropOnWidget(Widget);
	}

	return false;
}

void UHeartWidgetInputLinker::HandleNativeOnDragEnter(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent,
	UDragDropOperation* InOperation)
{
	// Nothing here yet
}

void UHeartWidgetInputLinker::HandleNativeOnDragLeave(UWidget* Widget, const FDragDropEvent& DragDropEvent,
	UDragDropOperation* InOperation)
{
	// Nothing here yet
}

void UHeartWidgetInputLinker::HandleNativeOnDragCancelled(UWidget* Widget, const FDragDropEvent& DragDropEvent,
	UDragDropOperation* InOperation)
{
	// Nothing here yet
}

FReply UHeartWidgetInputLinker::HandleManualInput(UWidget* Widget, /*const FGeometry& InGeometry,*/
												  const FName Key, const FHeartManualEvent& Activation)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleNativeOnDrop)

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

TArray<FHeartManualInputQueryResult> UHeartWidgetInputLinker::QueryManualTriggers(const UWidget* Widget) const
{
	TArray<FHeartManualInputQueryResult> Results;

	for (auto&& ConditionalInputCallback : InputCallbackMappings)
	{
		if (ConditionalInputCallback.Key.Type != Manual)
		{
			continue;
		}

		bool PassedCondition = true;

		if (const auto ConditionCallback = static_cast<TLinkerType<UWidget>::FConditionDelegate*>(ConditionalInputCallback.Value.Condition.Get());
			ConditionCallback->IsBound())
		{
			PassedCondition = ConditionCallback->Execute(Widget);
		}

		if (!PassedCondition)
		{
			continue;
		}

		if (const auto DescriptionCallback = static_cast<TLinkerType<UWidget>::FDescriptionDelegate*>(ConditionalInputCallback.Value.Description.Get());
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

void UHeartWidgetInputLinker::BindToOnDragDetected(const FInputTrip& Trip, const FConditionalCallback_DDO& DragDropTrigger)
{
	if (ensure(Trip.IsValid()))
	{
		DragDropTriggers.Add(Trip, DragDropTrigger);
	}
}

void UHeartWidgetInputLinker::UnbindToOnDragDetected(const FInputTrip& Trip)
{
	DragDropTriggers.Remove(Trip);
}

bool UHeartWidgetInputLinker::TriggerManualInput(UWidget* Widget, const FName Key, const FHeartManualEvent& Activation)
{
	if (!IsValid(Widget) || Key.IsNone()) return false;

	return HandleManualInput(Widget, Key, Activation).IsEventHandled();
}