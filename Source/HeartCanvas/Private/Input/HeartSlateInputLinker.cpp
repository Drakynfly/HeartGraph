// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartSlateInputLinker.h"
#include "Input/HeartDragDropOperation.h"
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
	const FHeartEvent Reply = QuickTryCallbacks(FInputTrip(HackedPointerEvent, Press), Wrapper, HackedPointerEvent);
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

	FReply Reply = FReply::Unhandled();

	const FHeartInputActivation Activation = PointerEvent;

	Query(FInputTrip(PointerEvent, Press)).ForEachWithBreak(Wrapper,
		[&](const FConditionalCallback& Ref)
		{
			if (Ref.Priority == Deferred)
			{
				if (const TSharedPtr<SWidget> SlateWidgetDetectingDrag = Widget;
					SlateWidgetDetectingDrag.IsValid())
				{
					Reply = FReply::Handled().DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), PointerEvent.GetEffectingButton());
					return false;
				}
				return true;
			}

			const FHeartEvent Event = Ref.Handler.Execute(Wrapper, Activation);

			// If Priority is Event, this event Reply is allowed to stop capture input, and break out of the input handling loop
			if (Ref.Priority <= HighestHandlingPriority)
			{
				if (Event.WasEventCaptured())
				{
					if (auto EventReply = Event.As<FEventReply>();
						EventReply.IsSet())
					{
						Reply = EventReply.GetValue().NativeReply;
						return false;
					}
				}
			}

			return true;
		});

	return Reply;
}

FReply UHeartSlateInputLinker::HandleOnMouseButtonUp(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FPointerEvent& PointerEvent)
{
	auto&& Wrapper = UHeartSlatePtr::Wrap(Widget);

	const FHeartEvent Reply = QuickTryCallbacks(FInputTrip(PointerEvent, Release), Wrapper, PointerEvent);
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

	const FHeartEvent Reply = QuickTryCallbacks(FInputTrip(KeyEvent, Press), Wrapper, KeyEvent);
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

	const FHeartEvent Reply = QuickTryCallbacks(FInputTrip(KeyEvent, Release), Wrapper, KeyEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnDragDetected(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry,
	const FPointerEvent& MouseEvent)
{
	auto&& Wrapper = UHeartSlatePtr::Wrap(Widget);

	UHeartSlateDragDropOperation* OperationWrapper = nullptr;

	const FHeartInputActivation Activation = MouseEvent;

	Query(FInputTrip(MouseEvent, Press))
		.ForEachWithBreak(Wrapper,
		[&](const FConditionalCallback& Ref)
		{
			const FHeartEvent HandlerEvent = Ref.Handler.Execute(Wrapper, Activation);
			if (auto Option = HandlerEvent.As<FHeartDeferredEvent>();
				Option.IsSet())
			{
				OperationWrapper = Cast<UHeartSlateDragDropOperation>(Option.GetValue().Handler.GetObject());
				if (IsValid(OperationWrapper))
				{
					return false;
				}
			}

			return true;
		});

	if (IsValid(OperationWrapper) && OperationWrapper->SlatePointer.IsValid())
	{
		return FReply::Handled().BeginDragDrop(OperationWrapper->SlatePointer.ToSharedRef());
	}

	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnDrop(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry,
	const FDragDropEvent& DragDropEvent)
{
	return DragDropEvent.GetOperationAs<Heart::FNativeDragDropOperation>()->OnHoverWidget(Widget);
}

FReply UHeartSlateInputLinker::HandleOnDragOver(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry,
	const FDragDropEvent& DragDropEvent)
{
	return DragDropEvent.GetOperationAs<Heart::FNativeDragDropOperation>()->OnHoverWidget(Widget);
}

void UHeartSlateInputLinker::HandleOnDragEnter(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry,
	const FDragDropEvent& DragDropEvent)
{
	// Nothing here yet
}

void UHeartSlateInputLinker::HandleOnDragLeave(const TSharedRef<SWidget>& Widget, const FDragDropEvent& DragDropEvent)
{
	// Nothing here yet
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