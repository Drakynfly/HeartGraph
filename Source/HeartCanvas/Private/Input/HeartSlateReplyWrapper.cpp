// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartSlateReplyWrapper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSlateReplyWrapper)

namespace Heart::Input
{
	FReply HeartEventToReply(const FHeartEvent& Event)
	{
		// If we got a detailed reply, use that first
		if (auto EventReply = Event.As<FEventReply>();
			EventReply.IsSet())
		{
			return EventReply.GetValue().NativeReply;
		}

		// If we got a non-specific, but capturing reply return handled, otherwise nothing was handled, and bubble input
		return Event.WasEventCaptured() ?
			FReply::Handled() :
			FReply::Unhandled();
	}

	FHeartEvent ReplyToHeartEvent(const FHeartEvent& Base, const FReply& Reply)
	{
		FEventReply EventReply;
		EventReply.NativeReply = Reply;

		// If the slate reply is handling, enforce using Handled instead of the passed in Reply
		if (Reply.IsEventHandled())
		{
			return FHeartEvent::Handled.Detail<FEventReply>(EventReply);
		}

		// Otherwise, Reply has more responses than just "Unhandled"
		return Base.Detail<FEventReply>(EventReply);
	}
}

FEventReply UHeartSlateReplyWrapper::HeartEventToEventReply(const FHeartEvent& Event)
{
	FEventReply EventReply;
	EventReply.NativeReply = Heart::Input::HeartEventToReply(Event);
	return EventReply;
}

FHeartEvent UHeartSlateReplyWrapper::ReplyEventToHeartEvent(const FHeartEvent& Event, const FEventReply& EventReply)
{
	// If the slate reply is handling, enforce using Handled instead of the passed in Reply
	if (EventReply.NativeReply.IsEventHandled())
	{
		return FHeartEvent::Handled.Detail<FEventReply>(EventReply);
	}

	// Otherwise, Reply has more responses than just "Unhandled"
	return Event.Detail<FEventReply>(EventReply);
}

bool UHeartSlateReplyWrapper::IsAnEventReply(const FHeartEvent& Event)
{
	return Event.GetScriptStruct() == FEventReply::StaticStruct();
}