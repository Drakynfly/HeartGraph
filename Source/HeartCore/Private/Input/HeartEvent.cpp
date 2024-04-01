// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartEvent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartEvent)

const FHeartEvent FHeartEvent::Invalid(-1);
const FHeartEvent FHeartEvent::Ignored(0);
const FHeartEvent FHeartEvent::Handled(1);
const FHeartEvent FHeartEvent::Failed(2);

bool FHeartEvent::WasEventInvalid() const
{
	return -1 == Status;
}

bool FHeartEvent::WasEventIgnored() const
{
	return 0 == Status;
}

bool FHeartEvent::WasEventHandled() const
{
	return 1 == Status;
}

bool FHeartEvent::DidHandlingFail() const
{
	return 2 == Status;
}

bool UHeartEventLibrary::WasEventInvalid(const FHeartEvent& Event)
{
	return Event.WasEventInvalid();
}

bool UHeartEventLibrary::WasEventIgnored(const FHeartEvent& Event)
{
	return Event.WasEventIgnored();
}

bool UHeartEventLibrary::WasEventHandled(const FHeartEvent& Event)
{
	return Event.WasEventHandled();
}

bool UHeartEventLibrary::DidHandlingFail(const FHeartEvent& Event)
{
	return Event.DidHandlingFail();
}

bool UHeartEventLibrary::WasEventSuccessful(const FHeartEvent& Event)
{
	return Event.WasEventSuccessful();
}

bool UHeartEventLibrary::WasEventCaptured(const FHeartEvent& Event)
{
	return Event.WasEventCaptured();
}

FHeartEvent UHeartEventLibrary::MakeDeferredEvent(const FHeartEvent& Event, const TScriptInterface<IHeartDeferredEventHandler> Handler)
{
	return Event.Detail<FHeartDeferredEvent>(Handler);
}

bool UHeartEventLibrary::IsDeferredEvent(const FHeartEvent& Event)
{
	return Event.GetScriptStruct() == FHeartDeferredEvent::StaticStruct();
}