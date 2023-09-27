// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputLinker.h"

#include "Components/Widget.h"
#include "HeartCorePrivate.h"

#include "UI/HeartInputActivation.h"
#include "UI/HeartUMGContextObject.h"
#include "UI/HeartWidgetInputBindingAsset.h"
#include "UI/HeartWidgetInputHandlerAsset.h"

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

FReply UHeartWidgetInputLinker::HandleOnMouseWheel(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseWheel)

	FInputTrip MouseWheelAxisTrip;
	MouseWheelAxisTrip.Key = EKeys::MouseWheelAxis;
	MouseWheelAxisTrip.Type = Press; // Mouse wheel events must always use the 'Press' type

	FPointerEvent ActivationEvent(
		PointerEvent.GetUserIndex(),
		PointerEvent.GetPointerIndex(),
		PointerEvent.GetScreenSpacePosition(),
		PointerEvent.GetLastScreenSpacePosition(),
		PointerEvent.GetPressedButtons(),
		EKeys::MouseWheelAxis,
		PointerEvent.GetWheelDelta(),
		PointerEvent.GetModifierKeys());

	TArray<FConditionalInputCallback> Callbacks;
	InputCallbackMappings.MultiFind(MouseWheelAxisTrip, Callbacks);
	Callbacks.Sort();
	for (auto&& ConditionalInputCallback : Callbacks)
	{
		bool PassedCondition = true;

		if (ConditionalInputCallback.Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback.Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalInputCallback.Callback.IsBound())
		{
			FReply Reply = ConditionalInputCallback.Callback.Execute(Widget, FHeartInputActivation(ActivationEvent));

			if (ConditionalInputCallback.Layer == Event)
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

FReply UHeartWidgetInputLinker::HandleOnMouseButtonDown(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseButtonDown)

	FInputTrip Trip;
	Trip.Key = PointerEvent.GetEffectingButton().IsValid() ? PointerEvent.GetEffectingButton() : *PointerEvent.GetPressedButtons().CreateConstIterator();
	Trip.Type = Press;

	TArray<FConditionalInputCallback> Callbacks;
	InputCallbackMappings.MultiFind(Trip, Callbacks);
	Callbacks.Sort();
	for (auto&& ConditionalInputCallback : Callbacks)
	{
		bool PassedCondition = true;

		if (ConditionalInputCallback.Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback.Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalInputCallback.Callback.IsBound())
		{
			FReply Reply = ConditionalInputCallback.Callback.Execute(Widget, FHeartInputActivation(PointerEvent));

			if (ConditionalInputCallback.Layer == Event)
			{
				if (Reply.IsEventHandled())
				{
					return Reply;
				}
			}
		}
	}

	TArray<FConditionalDragDropTrigger> DropDropTriggerArray;
	DragDropTriggers.MultiFind(Trip, DropDropTriggerArray);
	DropDropTriggerArray.Sort();
	for (auto&& ConditionalDropDropTrigger : DropDropTriggerArray)
	{
		bool PassedCondition = true;

		if (ConditionalDropDropTrigger.Condition.IsBound())
		{
			PassedCondition = ConditionalDropDropTrigger.Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalDropDropTrigger.Callback.IsBound())
		{
			const TSharedPtr<SWidget> SlateWidgetDetectingDrag = Widget->GetCachedWidget();
			if (SlateWidgetDetectingDrag.IsValid())
			{
				FReply Reply = FReply::Handled().DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), PointerEvent.GetEffectingButton());

				if (ConditionalDropDropTrigger.Layer == Event)
				{
					if (Reply.IsEventHandled())
					{
						return Reply;
					}
				}
			}
		}
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnMouseButtonUp(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseButtonUp)

	FInputTrip Trip;
	Trip.Key = PointerEvent.GetEffectingButton().IsValid() ? PointerEvent.GetEffectingButton() : *PointerEvent.GetPressedButtons().CreateConstIterator();
	Trip.Type = Release;

	TArray<FConditionalInputCallback> Callbacks;
	InputCallbackMappings.MultiFind(Trip, Callbacks);
	Callbacks.Sort();
	for (auto&& ConditionalInputCallback : Callbacks)
	{
		bool PassedCondition = true;

		if (ConditionalInputCallback.Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback.Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalInputCallback.Callback.IsBound())
		{
			FReply Reply = ConditionalInputCallback.Callback.Execute(Widget, FHeartInputActivation(PointerEvent));

			if (ConditionalInputCallback.Layer == Event)
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

FReply UHeartWidgetInputLinker::HandleOnKeyDown(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleOnKeyDown)

	FInputTrip Trip;
	Trip.Key = KeyEvent.GetKey();
	Trip.Type = Press;

	TArray<FConditionalInputCallback> Callbacks;
	InputCallbackMappings.MultiFind(Trip, Callbacks);
	Callbacks.Sort();
	for (auto&& ConditionalInputCallback : Callbacks)
	{
		bool PassedCondition = true;

		if (ConditionalInputCallback.Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback.Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalInputCallback.Callback.IsBound())
		{
			FReply Reply = ConditionalInputCallback.Callback.Execute(Widget, FHeartInputActivation(KeyEvent));

			if (ConditionalInputCallback.Layer == Event)
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

FReply UHeartWidgetInputLinker::HandleOnKeyUp(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleOnKeyUp)

	FInputTrip Trip;
	Trip.Key = KeyEvent.GetKey();
	Trip.Type = Release;

	TArray<FConditionalInputCallback> Callbacks;
	InputCallbackMappings.MultiFind(Trip, Callbacks);
	Callbacks.Sort();
	for (auto&& ConditionalInputCallback : Callbacks)
	{
		bool PassedCondition = true;

		if (ConditionalInputCallback.Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback.Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalInputCallback.Callback.IsBound())
		{
			FReply Reply = ConditionalInputCallback.Callback.Execute(Widget, FHeartInputActivation(KeyEvent));

			if (ConditionalInputCallback.Layer == Event)
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

UHeartDragDropOperation* UHeartWidgetInputLinker::HandleOnDragDetected(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleOnDragDetected)

	FInputTrip Trip;
	Trip.Type = Press;
	Trip.Key = PointerEvent.GetEffectingButton().IsValid() ? PointerEvent.GetEffectingButton() : *PointerEvent.GetPressedButtons().CreateConstIterator();

	TArray<FConditionalDragDropTrigger> DropDropTriggerArray;
	DragDropTriggers.MultiFind(Trip, DropDropTriggerArray);
	DropDropTriggerArray.Sort();
	for (auto&& DragDropTrigger : DropDropTriggerArray)
	{
		bool PassedCondition = true;

		if (DragDropTrigger.Condition.IsBound())
		{
			PassedCondition = DragDropTrigger.Condition.Execute(Widget);
		}

		if (PassedCondition && DragDropTrigger.Callback.IsBound())
		{
			UHeartDragDropOperation* DragDropOperation = DragDropTrigger.Callback.Execute(Widget);

			if (!IsValid(DragDropOperation)) continue;

			DragDropOperation->SummonedBy = Widget;

			if (Widget->Implements<UHeartUMGContextObject>())
			{
				DragDropOperation->Payload = IHeartUMGContextObject::Execute_GetContextObject(Widget);
			}

			// @todo its a little bogus to create the ddo then call this every time to see if it will be handled :/
			if (DragDropOperation->SetupDragDropOperation())
			{
				return DragDropOperation;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Created DDO (%s) unnecessarily, figure out why"), *DragDropOperation->GetClass()->GetName())
			}
		}
	}

	return nullptr;
}

bool UHeartWidgetInputLinker::HandleNativeOnDragOver(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleNativeOnDragOver)

	if (auto&& HeartDDO = Cast<UHeartDragDropOperation>(InOperation))
	{
		return HeartDDO->OnHoverWidget(Widget);
	}

	return false;
}

bool UHeartWidgetInputLinker::HandleNativeOnDrop(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent,
												 UDragDropOperation* InOperation)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleNativeOnDrop)

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

FReply UHeartWidgetInputLinker::HandleManualInput(UWidget* Widget, /*const FGeometry& InGeometry,*/ FName Key, const FHeartManualEvent& Activation)
{
	SCOPE_CYCLE_COUNTER(STAT_HandleNativeOnDrop)

	FInputTrip Trip;
	Trip.Type = Manual;
	Trip.CustomKey = Key;

	TArray<FConditionalInputCallback> Callbacks;
	InputCallbackMappings.MultiFind(Trip, Callbacks);
	Callbacks.Sort();
	for (auto&& ConditionalInputCallback : Callbacks)
	{
		bool PassedCondition = true;

		if (ConditionalInputCallback.Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback.Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalInputCallback.Callback.IsBound())
		{
			FReply Reply = ConditionalInputCallback.Callback.Execute(Widget, FHeartInputActivation(Activation));

			if (ConditionalInputCallback.Layer == Event)
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

		if (ConditionalInputCallback.Value.Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback.Value.Condition.Execute(Widget);
		}

		if (PassedCondition)
		{
			if (ConditionalInputCallback.Value.Description.IsBound())
			{
				Results.Add({ConditionalInputCallback.Key.CustomKey, ConditionalInputCallback.Value.Description.Execute(Widget)});
			}
			else
			{
				Results.Add({ConditionalInputCallback.Key.CustomKey});
			}
		}
	}

	return Results;
}

void UHeartWidgetInputLinker::BindInputCallback(const FInputTrip& Trip, const FConditionalInputCallback& InputCallback)
{
	if (ensure(Trip.IsValid()))
	{
		InputCallbackMappings.Add(Trip, InputCallback);
	}
}

void UHeartWidgetInputLinker::UnbindInputCallback(const FInputTrip& Trip)
{
	InputCallbackMappings.Remove(Trip);

}

void UHeartWidgetInputLinker::BindToOnDragDetected(const FInputTrip& Trip, const FConditionalDragDropTrigger& DragDropTrigger)
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

void UHeartWidgetInputLinker::AddBindings(const TArray<FHeartWidgetInputBinding>& Bindings)
{
	for (auto&& Binding : Bindings)
	{
		if (!IsValid(Binding.InputHandler)) continue;

		Binding.InputHandler->Bind(this, Binding.Triggers);
	}
}

void UHeartWidgetInputLinker::RemoveBindings(const TArray<FHeartWidgetInputBinding>& Bindings)
{
	for (auto&& Binding : Bindings)
	{
		if (!IsValid(Binding.InputHandler)) continue;

		Binding.InputHandler->Unbind(this, Binding.Triggers);
	}
}

bool UHeartWidgetInputLinker::TriggerManualInput(UWidget* Widget, const FName Key, const FHeartManualEvent& Activation)
{
	if (!IsValid(Widget) || Key.IsNone()) return false;

	return HandleManualInput(Widget, Key, Activation).IsEventHandled();
}