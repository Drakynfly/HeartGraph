// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBindingContainer.h"
#include "UI/HeartInputActivation.h"
#include "Blueprint/UserWidget.h"

namespace Heart::Input
{
	// Stolen from UWidgetInteractionComponent
	void GetKeyAndCharCodes(const FKey& Key, bool& bHasKeyCode, uint32& KeyCode, bool& bHasCharCode, uint32& CharCode)
	{
		const uint32* KeyCodePtr;
		const uint32* CharCodePtr;
		FInputKeyManager::Get().GetCodesFromKey(Key, KeyCodePtr, CharCodePtr);

		bHasKeyCode = KeyCodePtr ? true : false;
		bHasCharCode = CharCodePtr ? true : false;

		KeyCode = KeyCodePtr ? *KeyCodePtr : 0;
		CharCode = CharCodePtr ? *CharCodePtr : 0;

		// These special keys are not handled by the platform layer, and while not printable
		// have character mappings that several widgets look for, since the hardware sends them.
		if (CharCodePtr == nullptr)
		{
			if (Key == EKeys::Tab)
			{
				CharCode = '\t';
				bHasCharCode = true;
			}
			else if (Key == EKeys::BackSpace)
			{
				CharCode = '\b';
				bHasCharCode = true;
			}
			else if (Key == EKeys::Enter)
			{
				CharCode = '\n';
				bHasCharCode = true;
			}
		}
	}

	FKeyEvent MakeKeyEventFromKey(const FKey& Key)
	{
		bool bHasKeyCode, bHasCharCode;
		uint32 KeyCode, CharCode;
		GetKeyAndCharCodes(Key, bHasKeyCode, KeyCode, bHasCharCode, CharCode);

		return FKeyEvent(Key, FModifierKeysState(), 0, false, KeyCode, CharCode);
	}
}


FReply UHeartWidgetInputLinker::HandleOnMouseWheel(UWidget* Widget, const FPointerEvent& PointerEvent)
{
	FHeartWidgetInputTrip MouseWheelAxisTrip;
	MouseWheelAxisTrip.Key = EKeys::MouseWheelAxis; // Heart::Input::MakeKeyEventFromKey(EKeys::MouseWheelAxis);

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
			return ConditionalInputCallback->Callback.Execute(Widget, Activation);
		}
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnMouseButtonDown(UWidget* Widget, const FPointerEvent& PointerEvent)
{
	FHeartWidgetInputTrip Trip;
	Trip.Key = PointerEvent.GetEffectingButton().IsValid() ? PointerEvent.GetEffectingButton() : *PointerEvent.GetPressedButtons().CreateConstIterator();

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
			return ConditionalInputCallback->Callback.Execute(Widget, Activation);
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
				return FReply::Handled().DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), PointerEvent.GetEffectingButton());
			}
		}
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnKeyDown(UWidget* Widget, const FKeyEvent& KeyEvent)
{
	FHeartWidgetInputTrip Trip;
	Trip.Key = KeyEvent.GetKey();

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
			return ConditionalInputCallback->Callback.Execute(Widget, Activation);
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

void UHeartWidgetInputLinker::BindToOnKeyDown(const FHeartWidgetInputTrip& Trip, const FConditionalInputCallback& InputCallback)
{
	InputCallbackMappings.Add(Trip, InputCallback);
}

void UHeartWidgetInputLinker::UnbindToOnKeyDown(const FHeartWidgetInputTrip& Trip)
{
	InputCallbackMappings.Remove(Trip);
}

void UHeartWidgetInputLinker::BindToOnDragDetected(const FHeartWidgetInputTrip& Trip, const FConditionalDragDropTriggers& DragDropTrigger)
{
	DragDropTriggers.Add(Trip, DragDropTrigger);
}

void UHeartWidgetInputLinker::UnbindToOnDragDetected(const FHeartWidgetInputTrip& Trip)
{
	DragDropTriggers.Remove(Trip);
}

FHeartWidgetInputTrip FHeartWidgetInputTrigger_Key::CreateTrip() const
{
	FHeartWidgetInputTrip Trip;
	Trip.Key = Key;
	//Trip.KeyEvent = Heart::Input::MakeKeyEventFromKey(Key);
	return Trip;
}

FHeartWidgetInputCondition UHeartWidgetInputCondition_WidgetClass::CreateCondition() const
{
	return FHeartWidgetInputCondition::CreateLambda([=](UWidget* Widget)
	{
		return Widget && Widget->GetClass()->IsChildOf(WidgetClass);
	});
}

bool UHeartWidgetInputBinding_TriggerBase::Bind(UHeartWidgetInputLinker* Linker)
{
	auto&& Delegate = Event->CreateDelegate();

	for (auto&& Trigger : Triggers)
	{
		UHeartWidgetInputLinker::FConditionalInputCallback InputCallback;
		InputCallback.Callback = Delegate;

		if (Condition)
		{
			InputCallback.Condition = Condition->CreateCondition();
		}

		Linker->BindToOnKeyDown(Trigger.GetMutable<FHeartWidgetInputTrigger>().CreateTrip(), InputCallback);
	}

	return true;
}

bool UHeartWidgetInputBinding_TriggerBase::Unbind(UHeartWidgetInputLinker* Linker)
{
	for (auto&& Trigger : Triggers)
	{
		Linker->UnbindToOnKeyDown(Trigger.GetMutable<FHeartWidgetInputTrigger>().CreateTrip());
	}

	return true;
}

bool UHeartWidgetInputBinding_DragDropOperation::Bind(UHeartWidgetInputLinker* Linker)
{
	for (auto&& Trigger : Triggers)
	{
		UHeartWidgetInputLinker::FConditionalDragDropTriggers DragDropTrigger;
		DragDropTrigger.Class = OperationClass;
		DragDropTrigger.VisualClass = VisualClass;
		DragDropTrigger.Pivot = Pivot;
		DragDropTrigger.Offset = Offset;

		if (Condition)
		{
			DragDropTrigger.Condition = Condition->CreateCondition();
		}

		Linker->BindToOnDragDetected(Trigger.Get<FHeartWidgetInputTrigger>().CreateTrip(), DragDropTrigger);
	}

	return true;
}

bool UHeartWidgetInputBinding_DragDropOperation::Unbind(UHeartWidgetInputLinker* Linker)
{
	for (auto&& Trigger : Triggers)
	{
		Linker->UnbindToOnDragDetected(Trigger.GetMutable<FHeartWidgetInputTrigger>().CreateTrip());
	}

	return true;}

void FHeartWidgetInputBindingContainer::SetLinker(UHeartWidgetInputLinker* InLinker)
{
	// Unbind everything from the current linker
	if (Linker)
	{
		for (auto&& Binding : Bindings)
        {
        	if (Binding)
        	{
        		Binding->Unbind(Linker);
        	}
        }
	}

	Linker = InLinker;

	// Rebind everything to the new linker
	for (auto&& Binding : Bindings)
	{
		if (Binding)
		{
			Binding->Bind(Linker);
		}
	}
}

FHeartWidgetLinkedInput UNamedFunctionEvent::CreateDelegate() const
{
	return FHeartWidgetLinkedInput::CreateLambda([this](UWidget* Widget, const FHeartInputActivation& Activation)
	{
		if (Widget)
		{
			const FName FunctionFName(*FunctionName);

			UFunction* const Func = Widget->FindFunction(FunctionFName);
			if (Func && (Func->ParmsSize > 0))
			{
				// User passed in a valid function, but one that takes parameters
				// FTimerDynamicDelegate expects zero parameters and will choke on execution if it tries
				// to execute a mismatched function
				UE_LOG(LogBlueprintUserMessages, Warning, TEXT("AddNamedFunction passed a function (%s) that expects parameters."), *FunctionName);
				return FReply::Unhandled();
			}

			void* Parameters;
			Widget->ProcessEvent(Func, &Parameters);
			return FReply::Handled();
		}

		return FReply::Unhandled();
	});
}