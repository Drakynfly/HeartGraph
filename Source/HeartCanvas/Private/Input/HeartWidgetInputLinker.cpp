// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartWidgetInputLinker.h"
#include "Components/Widget.h"

#include "Input/HeartDragDropOperation.h"
#include "Input/HeartInputActivation.h"
#include "Input/HeartInputLinkerInterface.h"
#include "Input/HeartInputTypes.h"
#include "Input/HeartEvent.h"
#include "Input/HeartSlateReplyWrapper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartWidgetInputLinker)

using namespace Heart::Input;

FReply UHeartWidgetInputLinker::HandleOnMouseWheel(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseWheel)

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
	const FHeartEvent Reply = QuickTryCallbacks(FInputTrip(HackedPointerEvent, Press), Widget, HackedPointerEvent);

	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnMouseButtonDown(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseButtonDown)

	FReply Reply = FReply::Unhandled();

	const FHeartInputActivation Activation = PointerEvent;

	Query(FInputTrip(PointerEvent, Press))
		.ForEachWithBreak(Widget,
		[&](const FConditionalCallback& Ref)
		{
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

			const FHeartEvent Event = Ref.Handler.Execute(Widget, Activation);

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

FReply UHeartWidgetInputLinker::HandleOnMouseButtonUp(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseButtonUp)

	const FHeartEvent Reply = QuickTryCallbacks(FInputTrip(PointerEvent, Release), Widget, PointerEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnKeyDown(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnKeyDown)

	const FHeartEvent Reply = QuickTryCallbacks(FInputTrip(KeyEvent, Press), Widget, KeyEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnKeyUp(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnKeyUp)

	const FHeartEvent Reply = QuickTryCallbacks(FInputTrip(KeyEvent, Release), Widget, KeyEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
}

UDragDropOperation* UHeartWidgetInputLinker::HandleOnDragDetected(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnDragDetected)

	UDragDropOperation* Operation = nullptr;

	const FHeartInputActivation Activation = PointerEvent;

	Query(FInputTrip(PointerEvent, Press))
		.ForEachWithBreak(Widget,
		[&](const FConditionalCallback& Ref)
		{
			const FHeartEvent HandlerEvent = Ref.Handler.Execute(Widget, PointerEvent);
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
		return HeartDDO->OnDropOnWidget(Widget);
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

namespace Heart::Input
{
	UHeartWidgetInputLinker* TLinkerType<UWidget>::FindLinker(const UWidget* Widget)
	{
		return TryFindLinker<UHeartWidgetInputLinker>(Widget);
	}
}