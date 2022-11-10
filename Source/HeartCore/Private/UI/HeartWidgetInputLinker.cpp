// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputLinker.h"

#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"

FReply UHeartWidgetInputLinker::HandleOnMouseWheel(UWidget* Widget, const FPointerEvent& PointerEvent)
{
	FHeartWidgetInputTrip MouseWheelAxisTrip;
	MouseWheelAxisTrip.Key = EKeys::MouseWheelAxis; // Heart::Input::MakeKeyEventFromKey(EKeys::MouseWheelAxis);
	MouseWheelAxisTrip.Release = false;

	if (auto&& ConditionalInputCallback = InputCallbackMappings.Find(MouseWheelAxisTrip))
	{
		bool PassedCondition = true;

		if (ConditionalInputCallback->Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback->Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalInputCallback->Callback.IsBound())
		{
			FHeartInputActivation Activation;
			Activation.ActivationValue = PointerEvent.GetWheelDelta();
			FReply Reply = ConditionalInputCallback->Callback.Execute(Widget, Activation);

			if (Reply.IsEventHandled())
			{
				return Reply;
			}
		}
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnMouseButtonDown(UWidget* Widget, const FPointerEvent& PointerEvent)
{
	FHeartWidgetInputTrip Trip;
	Trip.Key = PointerEvent.GetEffectingButton().IsValid() ? PointerEvent.GetEffectingButton() : *PointerEvent.GetPressedButtons().CreateConstIterator();
	Trip.Release = false;

	if (auto&& ConditionalInputCallback = InputCallbackMappings.Find(Trip))
	{
		bool PassedCondition = true;

		if (ConditionalInputCallback->Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback->Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalInputCallback->Callback.IsBound())
		{
			FHeartInputActivation Activation;
			Activation.ActivationValue = 1;
			FReply Reply = ConditionalInputCallback->Callback.Execute(Widget, Activation);

			if (Reply.IsEventHandled())
			{
				return Reply;
			}
		}
	}

	if (auto&& ConditionalDropDropTrigger = DragDropTriggers.Find(Trip))
	{
		bool PassedCondition = true;

		if (ConditionalDropDropTrigger->Condition.IsBound())
		{
			PassedCondition = ConditionalDropDropTrigger->Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalDropDropTrigger->Class)
		{
			const TSharedPtr<SWidget> SlateWidgetDetectingDrag = Widget->GetCachedWidget();
			if (SlateWidgetDetectingDrag.IsValid())
			{
				FReply Reply = FReply::Handled().DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), PointerEvent.GetEffectingButton());

				if (Reply.IsEventHandled())
				{
					return Reply;
				}
			}
		}
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnMouseButtonUp(UWidget* Widget, const FPointerEvent& PointerEvent)
{
	FHeartWidgetInputTrip Trip;
	Trip.Key = PointerEvent.GetEffectingButton().IsValid() ? PointerEvent.GetEffectingButton() : *PointerEvent.GetPressedButtons().CreateConstIterator();
	Trip.Release = true;

	if (auto&& ConditionalInputCallback = InputCallbackMappings.Find(Trip))
	{
		bool PassedCondition = true;

		if (ConditionalInputCallback->Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback->Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalInputCallback->Callback.IsBound())
		{
			FHeartInputActivation Activation;
			Activation.ActivationValue = 0;
			FReply Reply = ConditionalInputCallback->Callback.Execute(Widget, Activation);

			if (Reply.IsEventHandled())
			{
				return Reply;
			}
		}
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnKeyDown(UWidget* Widget, const FKeyEvent& KeyEvent)
{
	FHeartWidgetInputTrip Trip;
	Trip.Key = KeyEvent.GetKey();
	Trip.Release = false;

	if (auto&& ConditionalInputCallback = InputCallbackMappings.Find(Trip))
	{
		bool PassedCondition = true;

		if (ConditionalInputCallback->Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback->Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalInputCallback->Callback.IsBound())
		{
			FHeartInputActivation Activation;
			Activation.ActivationValue = 1;
			FReply Reply = ConditionalInputCallback->Callback.Execute(Widget, Activation);

			if (Reply.IsEventHandled())
			{
				return Reply;
			}
		}
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnKeyUp(UWidget* Widget, const FKeyEvent& KeyEvent)
{
	FHeartWidgetInputTrip Trip;
	Trip.Key = KeyEvent.GetKey();
	Trip.Release = true;

	if (auto&& ConditionalInputCallback = InputCallbackMappings.Find(Trip))
	{
		bool PassedCondition = true;

		if (ConditionalInputCallback->Condition.IsBound())
		{
			PassedCondition = ConditionalInputCallback->Condition.Execute(Widget);
		}

		if (PassedCondition && ConditionalInputCallback->Callback.IsBound())
		{
			FHeartInputActivation Activation;
			Activation.ActivationValue = 0;
			FReply Reply = ConditionalInputCallback->Callback.Execute(Widget, Activation);

			if (Reply.IsEventHandled())
			{
				return Reply;
			}
		}
	}

	return FReply::Unhandled();
}

UHeartDragDropOperation* UHeartWidgetInputLinker::HandleOnDragDetected(UWidget* Widget, const FPointerEvent& PointerEvent)
{
	FHeartWidgetInputTrip Trip;
	Trip.Key = PointerEvent.GetEffectingButton().IsValid() ? PointerEvent.GetEffectingButton() : *PointerEvent.GetPressedButtons().CreateConstIterator();

	if (auto&& DropDropTrigger = DragDropTriggers.Find(Trip))
	{
		if (IsValid(DropDropTrigger->Class))
		{
			UHeartDragDropOperation* DragDropOperation = NewObject<UHeartDragDropOperation>(GetTransientPackage(), DropDropTrigger->Class);
			DragDropOperation->Payload = Widget;

			if (IsValid(DropDropTrigger->VisualClass))
			{
				DragDropOperation->DefaultDragVisual = CreateWidget(Widget, DropDropTrigger->VisualClass);
				DragDropOperation->Pivot = DropDropTrigger->Pivot;
				DragDropOperation->Offset = DropDropTrigger->Offset;
			}

			// @todo its a little bogus to create the ddo then call this every time to see if it will be handled :/
			if (DragDropOperation->SetupDragDropOperation())
			{
				return DragDropOperation;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Created DDO unnecessarily, figure out why"))
			}
		}
	}

	return nullptr;
}

bool UHeartWidgetInputLinker::HandleNativeOnDragOver(UWidget* Widget, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation)
{
	if (UHeartDragDropOperation* HeartDDO = Cast<UHeartDragDropOperation>(InOperation))
	{
		return HeartDDO->OnHoverWidget(Widget);
	}

	return false;
}

bool UHeartWidgetInputLinker::HandleNativeOnDrop(UWidget* Widget, const FDragDropEvent& DragDropEvent,
	UDragDropOperation* InOperation)
{
	if (UHeartDragDropOperation* HeartDDO = Cast<UHeartDragDropOperation>(InOperation))
	{
		return HeartDDO->CanDropOnWidget(Widget);
	}

	return false;
}

void UHeartWidgetInputLinker::BindInputCallback(const FHeartWidgetInputTrip& Trip, const FConditionalInputCallback& InputCallback)
{
	InputCallbackMappings.Add(Trip, InputCallback);
}

void UHeartWidgetInputLinker::UnbindInputCallback(const FHeartWidgetInputTrip& Trip)
{
	InputCallbackMappings.Remove(Trip);
}

void UHeartWidgetInputLinker::BindToOnDragDetected(const FHeartWidgetInputTrip& Trip, const FConditionalDragDropTrigger& DragDropTrigger)
{
	DragDropTriggers.Add(Trip, DragDropTrigger);
}

void UHeartWidgetInputLinker::UnbindToOnDragDetected(const FHeartWidgetInputTrip& Trip)
{
	DragDropTriggers.Remove(Trip);
}