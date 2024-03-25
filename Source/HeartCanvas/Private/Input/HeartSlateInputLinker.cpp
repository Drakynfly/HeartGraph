// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartSlateInputLinker.h"
#include "Input/HeartInputActivation.h"
#include "Slate/SHeartGraphWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSlateInputLinker)

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
	// @todo what the heck went on here? does PointerEvent have a different/null EffectingButton?
	const FPointerEvent HackedPointerEvent(
		PointerEvent.GetUserIndex(),
		PointerEvent.GetPointerIndex(),
		PointerEvent.GetScreenSpacePosition(),
		PointerEvent.GetLastScreenSpacePosition(),
		PointerEvent.GetPressedButtons(),
		EKeys::MouseWheelAxis,
		PointerEvent.GetWheelDelta(),
		PointerEvent.GetModifierKeys());

	// Mouse wheel events must always use the 'Press' type
	if (auto Result = TryCallbacks(FInputTrip(HackedPointerEvent, Press), Widget, HackedPointerEvent);
		Result.IsSet())
	{
		return Result.GetValue();
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnMouseButtonDown(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FPointerEvent& PointerEvent)
{
	if (auto Result = TryCallbacks(FInputTrip(PointerEvent, Press), Widget, PointerEvent);
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
	if (auto Result = TryCallbacks(FInputTrip(PointerEvent, Release), Widget, PointerEvent);
		Result.IsSet())
	{
		return Result.GetValue();
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnKeyDown(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FKeyEvent& KeyEvent)
{
	if (auto Result = TryCallbacks(FInputTrip(KeyEvent, Press), Widget, KeyEvent);
		Result.IsSet())
	{
		return Result.GetValue();
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnKeyUp(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FKeyEvent& KeyEvent)
{
	if (auto Result = TryCallbacks(FInputTrip(KeyEvent, Release), Widget, KeyEvent);
		Result.IsSet())
	{
		return Result.GetValue();
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleManualInput(const TSharedRef<SWidget>& Widget, const FName Key,
	const FHeartManualEvent& Activation)
{
	if (auto Result = TryCallbacks(FInputTrip(Key), Widget, Activation);
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

namespace Heart::Input
{
	UHeartSlateInputLinker* TLinkerType<SWidget>::FindLinker(const TSharedRef<SWidget>& Widget)
	{
		for (TSharedPtr<SWidget> Test = Widget; Test.IsValid(); Test = Test->GetParentWidget())
		{
			if (TSharedPtr<Canvas::FNodeAndLinkerMetadata> Metadata = Test->GetMetaData<Canvas::FNodeAndLinkerMetadata>();
				Metadata.IsValid())
			{
				return Metadata->Linker.Get();
			}
		}

		return nullptr;
	}
}