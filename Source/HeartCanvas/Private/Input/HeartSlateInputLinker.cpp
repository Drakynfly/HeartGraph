// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartSlateInputLinker.h"
#include "Input/HeartInputActivation.h"
#include "Input/HeartSlateReplyWrapper.h"
#include "Input/SlatePointerWrappers.h"
#include "Slate/SHeartGraphWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSlateInputLinker)

using namespace Heart::Input;

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

	auto&& Wrapper = UHeartSlatePtr::Wrap(Widget);

	// Mouse wheel events must always use the 'Press' type
	const FHeartEvent Reply = TryCallbacks(FInputTrip(HackedPointerEvent, Press), Wrapper, HackedPointerEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnMouseButtonDown(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FPointerEvent& PointerEvent)
{
	auto&& Wrapper = UHeartSlatePtr::Wrap(Widget);

	const FHeartEvent Reply = TryCallbacks(FInputTrip(PointerEvent, Press), Wrapper, PointerEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
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
	auto&& Wrapper = UHeartSlatePtr::Wrap(Widget);

	const FHeartEvent Reply = TryCallbacks(FInputTrip(PointerEvent, Release), Wrapper, PointerEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnKeyDown(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FKeyEvent& KeyEvent)
{
	auto&& Wrapper = UHeartSlatePtr::Wrap(Widget);

	const FHeartEvent Reply = TryCallbacks(FInputTrip(KeyEvent, Press), Wrapper, KeyEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnKeyUp(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FKeyEvent& KeyEvent)
{
	auto&& Wrapper = UHeartSlatePtr::Wrap(Widget);

	const FHeartEvent Reply = TryCallbacks(FInputTrip(KeyEvent, Release), Wrapper, KeyEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
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