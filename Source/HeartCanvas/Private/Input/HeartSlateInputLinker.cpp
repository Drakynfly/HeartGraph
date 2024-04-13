// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartSlateInputLinker.h"
#include "Input/HeartDragDropOperation.h"
#include "Input/HeartInputActivation.h"
#include "Input/HeartInputHandlerAssetBase.h"
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
	return HeartEventToReply(QuickTryCallbacks(FHeartInputTrip(HackedPointerEvent, Press), Wrapper, HackedPointerEvent));
}

FReply UHeartSlateInputLinker::HandleOnMouseButtonDown(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FPointerEvent& PointerEvent)
{
	auto&& Wrapper = UHeartSlatePtr::Wrap(Widget);

	FReply Reply = FReply::Unhandled();

	const FHeartInputActivation Activation = PointerEvent;

	Query(FHeartInputTrip(PointerEvent, Press))
		.ForEachWithBreak(Wrapper,
		[&](const FHeartSortableInputCallback& Ref)
		{
			// Slate interprets Deferred as launching a DragDropOperation
			if (Ref.Priority == EHeartInputExecutionOrder::Deferred)
			{
				if (const TSharedPtr<SWidget> SlateWidgetDetectingDrag = Widget;
					SlateWidgetDetectingDrag.IsValid())
				{
					Reply = FReply::Handled().DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), PointerEvent.GetEffectingButton());
					return false;
				}
				return true;
			}

			const FHeartEvent Event = Ref.Handler->OnTriggered(Wrapper, Activation);

			// If Priority is Event, this event Reply is allowed to stop capture input, and break out of the input handling loop
			if (Ref.Priority <= EHeartInputExecutionOrder::HighestHandlingPriority)
			{
				Reply = HeartEventToReply(Event);
				return !Reply.IsEventHandled();
			}

			return true;
		});

	return Reply;
}

FReply UHeartSlateInputLinker::HandleOnMouseButtonUp(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FPointerEvent& PointerEvent)
{
	auto&& Wrapper = UHeartSlatePtr::Wrap(Widget);
	return HeartEventToReply(QuickTryCallbacks(FHeartInputTrip(PointerEvent, Release), Wrapper, PointerEvent));
}

FReply UHeartSlateInputLinker::HandleOnKeyDown(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FKeyEvent& KeyEvent)
{
	auto&& Wrapper = UHeartSlatePtr::Wrap(Widget);
	return HeartEventToReply(QuickTryCallbacks(FHeartInputTrip(KeyEvent, Press), Wrapper, KeyEvent));
}

FReply UHeartSlateInputLinker::HandleOnKeyUp(const TSharedRef<SWidget>& Widget, const FGeometry& InGeometry,
	const FKeyEvent& KeyEvent)
{
	auto&& Wrapper = UHeartSlatePtr::Wrap(Widget);
	return HeartEventToReply(QuickTryCallbacks(FHeartInputTrip(KeyEvent, Release), Wrapper, KeyEvent));
}

FReply UHeartSlateInputLinker::HandleOnDragDetected(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry,
	const FPointerEvent& MouseEvent)
{
	auto&& Wrapper = UHeartSlatePtr::Wrap(Widget);

	UHeartSlateDragDropOperation* OperationWrapper = nullptr;

	const FHeartInputActivation Activation = MouseEvent;

	Query(FHeartInputTrip(MouseEvent, Press))
		.ForEachWithBreak(Wrapper,
		[&](const FHeartSortableInputCallback& Ref)
		{
			const FHeartEvent HandlerEvent = Ref.Handler->OnTriggered(Wrapper, Activation);
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
	if (auto NativeDDO = DragDropEvent.GetOperationAs<Heart::FNativeDragDropOperation>())
	{
		return NativeDDO->OnHoverWidget(Widget);
	}
	return FReply::Unhandled();
}

FReply UHeartSlateInputLinker::HandleOnDragOver(const TSharedRef<SWidget>& Widget, const FGeometry& MyGeometry,
	const FDragDropEvent& DragDropEvent)
{
	if (auto NativeDDO = DragDropEvent.GetOperationAs<Heart::FNativeDragDropOperation>())
	{
		return NativeDDO->OnHoverWidget(Widget);
	}
	return FReply::Unhandled();
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

UHeartSlateInputLinker* THeartInputLinkerType<SWidget>::FindLinker(const TSharedRef<SWidget>& Widget)
{
	for (TSharedPtr<SWidget> Test = Widget; Test.IsValid(); Test = Test->GetParentWidget())
	{
		if (TSharedPtr<Heart::Canvas::FLinkerMetadata> Metadata = Test->GetMetaData<Heart::Canvas::FLinkerMetadata>();
			Metadata.IsValid())
		{
			return Metadata->Linker.Get();
		}
	}

	return nullptr;
}