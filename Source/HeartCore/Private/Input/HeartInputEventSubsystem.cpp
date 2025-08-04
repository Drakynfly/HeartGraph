// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputEventSubsystem.h"
#include "Framework/Application/SlateApplication.h"
#include "Input/HeartCoreInputPreprocessor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputEventSubsystem)

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

bool UHeartInputEventSubsystem::HandleKeyDownEvent(const FSlateApplication& SlateApp, const FKeyEvent& KeyEvent)
{
	TArray<Heart::Input::FEventCallback> Delegates;
	Callbacks.MultiFind(EHeartInputEventType::ButtonDown, Delegates);

	for (auto&& Delegate : Delegates)
	{
		if (Delegate.KeyCallback.IsBound())
		{
			Delegate.KeyCallback.Execute(KeyEvent);

			if (Delegate.Once)
			{
				// Cleanup
				UnbindFromInputEvent(EHeartInputEventType::ButtonDown, Delegate);
			}
		}
	}

	return false;
}

bool UHeartInputEventSubsystem::HandleKeyUpEvent(const FSlateApplication& SlateApp, const FKeyEvent& KeyEvent)
{
	TArray<Heart::Input::FEventCallback> Delegates;
	Callbacks.MultiFind(EHeartInputEventType::ButtonUp, Delegates);

	for (auto&& Delegate : Delegates)
	{
		if (Delegate.KeyCallback.IsBound())
		{
			Delegate.KeyCallback.Execute(KeyEvent);

			if (Delegate.Once)
			{
				// Cleanup
				UnbindFromInputEvent(EHeartInputEventType::ButtonDown, Delegate);
			}
		}
	}

	return false;
}

bool UHeartInputEventSubsystem::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	TArray<Heart::Input::FEventCallback> Delegates;
	Callbacks.MultiFind(EHeartInputEventType::ButtonDown, Delegates);

	for (auto&& Delegate : Delegates)
	{
		if (Delegate.MouseCallback.IsBound())
		{
			Delegate.MouseCallback.Execute(MouseEvent);

			if (Delegate.Once)
			{
				// Cleanup
				UnbindFromInputEvent(EHeartInputEventType::ButtonDown, Delegate);
			}
		}
	}

	return false;
}

bool UHeartInputEventSubsystem::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	TArray<Heart::Input::FEventCallback> Delegates;
	Callbacks.MultiFind(EHeartInputEventType::ButtonUp, Delegates);

	for (auto&& Delegate : Delegates)
	{
		if (Delegate.MouseCallback.IsBound())
		{
			Delegate.MouseCallback.Execute(MouseEvent);

			if (Delegate.Once)
			{
				// Cleanup
				UnbindFromInputEvent(EHeartInputEventType::ButtonDown, Delegate);
			}
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

void UHeartInputEventSubsystem::BindToKeyEvent(const EHeartInputEventType EventType, const FHeartKeyEvent& Callback,
											   const bool Once)
{
	BindToInputEvent(EventType, Heart::Input::FEventCallback{Callback, {}, Once});
}

void UHeartInputEventSubsystem::UnbindFromKeyEvent(const EHeartInputEventType EventType, const FHeartKeyEvent& Callback)
{
	UnbindFromInputEvent(EventType, Heart::Input::FEventCallback{Callback, {}});
}

void UHeartInputEventSubsystem::BindToMouseEvent(const EHeartInputEventType EventType,
												 const FHeartMouseEvent& Callback, const bool Once)
{
	BindToInputEvent(EventType, Heart::Input::FEventCallback{{}, Callback, Once});
}

void UHeartInputEventSubsystem::UnbindFromMouseEvent(const EHeartInputEventType EventType,
                                                     const FHeartMouseEvent& Callback)
{
	UnbindFromInputEvent(EventType, Heart::Input::FEventCallback{{}, Callback});
}