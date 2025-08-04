// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "HeartInputEventSubsystem.generated.h"

struct FPointerEvent;
struct FKeyEvent;
class FSlateApplication;
class FHeartCoreInputPreprocessor;

UENUM(BlueprintType)
enum class EHeartInputEventType : uint8
{
	ButtonDown,
	ButtonUp,
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FHeartKeyEvent, const FKeyEvent&, KeyEvent);
DECLARE_DYNAMIC_DELEGATE_OneParam(FHeartMouseEvent, const FPointerEvent&, PointerEvent);

namespace Heart::Input
{
	struct FEventCallback
	{
		const FHeartKeyEvent KeyCallback;
		const FHeartMouseEvent MouseCallback;
		const bool Once;

		friend bool operator==(const FEventCallback& Lhs, const FEventCallback& Rhs)
		{
			return Lhs.KeyCallback == Rhs.KeyCallback &&
				Lhs.MouseCallback == Rhs.MouseCallback;
		}

		friend bool operator!=(const FEventCallback& Lhs, const FEventCallback& Rhs)
		{
			return !(Lhs == Rhs);
		}
	};
}


/**
 * A blueprint interface for using FHeartCoreInputPreprocessor
 */
UCLASS()
class HEARTCORE_API UHeartInputEventSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

	friend FHeartCoreInputPreprocessor;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
	/** Keyboard button press */
	bool HandleKeyDownEvent(const FSlateApplication& SlateApp, const FKeyEvent& KeyEvent);

	/** Keyboard button release */
	bool HandleKeyUpEvent(const FSlateApplication& SlateApp, const FKeyEvent& KeyEvent);

	/** Mouse button press */
	bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent);

	/** Mouse button release */
	bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent);

	void BindToInputEvent(EHeartInputEventType EventType, const Heart::Input::FEventCallback& EventCallback);

	void UnbindFromInputEvent(EHeartInputEventType EventType, const Heart::Input::FEventCallback& EventCallback);

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|InputEventSubsystem")
	void BindToKeyEvent(EHeartInputEventType EventType, const FHeartKeyEvent& Callback, bool Once);

	UFUNCTION(BlueprintCallable, Category = "Heart|InputEventSubsystem")
	void UnbindFromKeyEvent(EHeartInputEventType EventType, const FHeartKeyEvent& Callback);

	UFUNCTION(BlueprintCallable, Category = "Heart|InputEventSubsystem")
	void BindToMouseEvent(EHeartInputEventType EventType, const FHeartMouseEvent& Callback, bool Once);

	UFUNCTION(BlueprintCallable, Category = "Heart|InputEventSubsystem")
	void UnbindFromMouseEvent(EHeartInputEventType EventType, const FHeartMouseEvent& Callback);

private:
	TSharedPtr<FHeartCoreInputPreprocessor> HeartInputPreprocessor;

	TMultiMap<EHeartInputEventType, Heart::Input::FEventCallback> Callbacks;
};
