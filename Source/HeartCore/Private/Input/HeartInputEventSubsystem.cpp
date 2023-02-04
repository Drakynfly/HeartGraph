// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputEventSubsystem.h"
#include "Input/HeartCoreInputPreprocessor.h"

void UHeartInputEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	HeartInputPreprocessor = MakeShared<FHeartCoreInputPreprocessor>(*this);
	FSlateApplication::Get().RegisterInputPreProcessor(HeartInputPreprocessor, 0);
}

void UHeartInputEventSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(HeartInputPreprocessor);
	}
	HeartInputPreprocessor.Reset();
}

bool UHeartInputEventSubsystem::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	TArray<Heart::Input::FEventCallback> Delegates;
	Callbacks.MultiFind(EHeartInputEventType::MouseButtonDown, Delegates);

	for (auto&& Delegate : Delegates)
	{
		if (!Delegate.Callback.ExecuteIfBound(MouseEvent) || Delegate.Once)
		{
			// Cleanup
			UnbindFromInputEvent(EHeartInputEventType::MouseButtonDown, Delegate);
		}
	}

	return false;
}

bool UHeartInputEventSubsystem::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	TArray<Heart::Input::FEventCallback> Delegates;
	Callbacks.MultiFind(EHeartInputEventType::MouseButtonUp, Delegates);

	for (auto&& Delegate : Delegates)
	{
		if (!Delegate.Callback.ExecuteIfBound(MouseEvent) || Delegate.Once)
		{
			// Cleanup
			UnbindFromInputEvent(EHeartInputEventType::MouseButtonUp, Delegate);
		}
	}

	return false;
}

void UHeartInputEventSubsystem::BindToInputEvent(const EHeartInputEventType EventType,
                                                 const Heart::Input::FEventCallback& EventCallback)
{
	Callbacks.Add(EventType, EventCallback);
}

void UHeartInputEventSubsystem::UnbindFromInputEvent(const EHeartInputEventType EventType,
                                                     const Heart::Input::FEventCallback& EventCallback)
{
	Callbacks.RemoveSingle(EventType, EventCallback);
}

void UHeartInputEventSubsystem::BindToInputEvent(const EHeartInputEventType EventType,
												 const FHeartInputEvent& EventCallback, const bool Once)
{
	BindToInputEvent(EventType, Heart::Input::FEventCallback{EventCallback, Once});
}

void UHeartInputEventSubsystem::UnbindFromInputEvent(const EHeartInputEventType EventType,
                                                     const FHeartInputEvent& EventCallback)
{
	UnbindFromInputEvent(EventType, Heart::Input::FEventCallback{EventCallback});
}
