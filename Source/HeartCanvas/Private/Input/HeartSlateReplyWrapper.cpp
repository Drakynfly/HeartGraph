// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartSlateReplyWrapper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSlateReplyWrapper)

FEventReply UHeartSlateReplyWrapper::HeartEventToEventReply(const FHeartEvent& Event)
{
	if (auto Option = Event.As<FEventReply>();
		Option.IsSet())
	{
		return Option.GetValue();
	}

	return FEventReply(false);
}

FHeartEvent UHeartSlateReplyWrapper::ReplyEventToHeartEvent(const FHeartEvent& Event, const FEventReply& EventReply)
{
	// If the slate reply is handling, enforce using Handled instead of the passed in Reply
	if (EventReply.NativeReply.IsEventHandled())
	{
		return FHeartEvent::Handled.Detail<FEventReply>(EventReply);
	}

	// Otherwise, Reply has more responces than just "Unhandled"
	return Event.Detail<FEventReply>(EventReply);
}

bool UHeartSlateReplyWrapper::IsAnEventReply(const FHeartEvent& Event)
{
	return Event.GetScriptStruct() == FEventReply::StaticStruct();
}