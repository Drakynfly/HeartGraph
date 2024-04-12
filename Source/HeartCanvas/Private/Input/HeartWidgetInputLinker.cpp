// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartWidgetInputLinker.h"
#include "Components/Widget.h"

#include "Input/HeartDragDropOperation.h"
#include "Input/HeartInputActivation.h"
#include "Input/HeartInputLinkerInterface.h"
#include "Input/HeartInputTypes.h"
#include "Input/HeartEvent.h"
#include "Input/HeartInputHandlerAssetBase.h"
#include "Input/HeartSlateReplyWrapper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartWidgetInputLinker)

using namespace Heart::Input;

FReply UHeartWidgetInputLinker::HandleOnMouseWheel(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	// @todo what the heck went on here? does PointerEvent have a different/null EffectingButton?
	const FPointerEvent HackedPointerEvent = FPointerEvent(
		PointerEvent.GetUserIndex(),
		PointerEvent.GetPointerIndex(),
		PointerEvent.GetScreenSpacePosition(),
		PointerEvent.GetLastScreenSpacePosition(),
		PointerEvent.GetPressedButtons(),
		EKeys::MouseWheelAxis,
		PointerEvent.GetWheelDelta(),
		PointerEvent.GetModifierKeys());

	// Mouse wheel events must always use the 'Press' type
	return HeartEventToReply(QuickTryCallbacks(FInputTrip(HackedPointerEvent, Press), Widget, HackedPointerEvent));
}

FReply UHeartWidgetInputLinker::HandleOnMouseButtonDown(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	FReply Reply = FReply::Unhandled();

	const FHeartInputActivation Activation = PointerEvent;

	Query(FInputTrip(PointerEvent, Press))
		.ForEachWithBreak(Widget,
		[&](const FSortableCallback& Ref)
		{
			// UMG interprets Deferred as launching a DragDropOperation
			if (Ref.Priority == Deferred)
			{
				if (const TSharedPtr<SWidget> SlateWidgetDetectingDrag = Widget->GetCachedWidget();
					SlateWidgetDetectingDrag.IsValid())
				{
					Reply = FReply::Handled().DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), PointerEvent.GetEffectingButton());
					return false;
				}
				return true;
			}

			const FHeartEvent Event = Ref.Handler->OnTriggered(Widget, Activation);

			// If Priority is Event, this event Reply is allowed to stop capture input, and break out of the input handling loop
			if (Ref.Priority <= HighestHandlingPriority)
			{
				Reply = HeartEventToReply(Event);
				return !Reply.IsEventHandled();
			}

			return true;
		});

	return Reply;
}

FReply UHeartWidgetInputLinker::HandleOnMouseButtonUp(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	return HeartEventToReply(QuickTryCallbacks(FInputTrip(PointerEvent, Release), Widget, PointerEvent));
}

FReply UHeartWidgetInputLinker::HandleOnKeyDown(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent)
{
	return HeartEventToReply(QuickTryCallbacks(FInputTrip(KeyEvent, Press), Widget, KeyEvent));
}

FReply UHeartWidgetInputLinker::HandleOnKeyUp(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent)
{
	return HeartEventToReply(QuickTryCallbacks(FInputTrip(KeyEvent, Release), Widget, KeyEvent));
}

UDragDropOperation* UHeartWidgetInputLinker::HandleOnDragDetected(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	UDragDropOperation* Operation = nullptr;

	const FHeartInputActivation Activation = PointerEvent;

	Query(FInputTrip(PointerEvent, Press))
		.ForEachWithBreak(Widget,
		[&](const FSortableCallback& Ref)
		{
			const FHeartEvent HandlerEvent = Ref.Handler->OnTriggered(Widget, PointerEvent);
			if (auto Option = HandlerEvent.As<FHeartDeferredEvent>();
				Option.IsSet())
			{
				Operation = Cast<UDragDropOperation>(Option.GetValue().Handler.GetObject());
				if (IsValid(Operation))
				{
					return false;
				}
			}

			return true;
		});

	return Operation;
}

bool UHeartWidgetInputLinker::HandleOnDragOver(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation)
{
	if (auto&& HeartDDO = Cast<UHeartDragDropOperation>(InOperation))
	{
		return HeartDDO->OnHoverWidget(Widget);
	}

	return false;
}

bool UHeartWidgetInputLinker::HandleOnDrop(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent,
												 UDragDropOperation* InOperation)
{
	if (auto&& HeartDDO = Cast<UHeartDragDropOperation>(InOperation))
	{
		return HeartDDO->OnDropOnWidget(Widget);
	}

	return false;
}

void UHeartWidgetInputLinker::HandleOnDragEnter(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent,
	UDragDropOperation* InOperation)
{
	// Nothing here yet
}

void UHeartWidgetInputLinker::HandleOnDragLeave(UWidget* Widget, const FDragDropEvent& DragDropEvent,
	UDragDropOperation* InOperation)
{
	// Nothing here yet
}

void UHeartWidgetInputLinker::HandleOnDragCancelled(UWidget* Widget, const FDragDropEvent& DragDropEvent,
	UDragDropOperation* InOperation)
{
	// Nothing here yet
}

namespace Heart::Input
{
	UHeartWidgetInputLinker* TLinkerType<UWidget>::FindLinker(const UWidget* Widget)
	{
		return TryFindLinker<UHeartWidgetInputLinker>(Widget);
	}
}