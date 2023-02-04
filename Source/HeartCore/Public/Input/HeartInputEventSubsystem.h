// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartInputEventSubsystem.generated.h"

class FHeartCoreInputPreprocessor;

UENUM(BlueprintType)
enum class EHeartInputEventType : uint8
{
	MouseButtonDown,
	MouseButtonUp,
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FHeartInputEvent, const FPointerEvent&, MouseEvent);

namespace Heart::Input
{
	struct FEventCallback
	{
		const FHeartInputEvent Callback;
		const bool Once;

		friend bool operator==(const FEventCallback& Lhs, const FEventCallback& Rhs)
		{
			return Lhs.Callback == Rhs.Callback;
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
	/** Mouse button press */
	bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent);

	/** Mouse button release */
	bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent);

	void BindToInputEvent(EHeartInputEventType EventType, const Heart::Input::FEventCallback& EventCallback);

	void UnbindFromInputEvent(EHeartInputEventType EventType, const Heart::Input::FEventCallback& EventCallback);

	UFUNCTION(BlueprintCallable)
	void BindToInputEvent(EHeartInputEventType EventType, const FHeartInputEvent& EventCallback, bool Once);

	UFUNCTION(BlueprintCallable)
	void UnbindFromInputEvent(EHeartInputEventType EventType, const FHeartInputEvent& EventCallback);

private:
	TSharedPtr<FHeartCoreInputPreprocessor> HeartInputPreprocessor;

	TMultiMap<EHeartInputEventType, Heart::Input::FEventCallback> Callbacks;
};
