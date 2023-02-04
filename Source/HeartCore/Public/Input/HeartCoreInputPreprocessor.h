// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Framework/Application/IInputProcessor.h"

class UHeartInputEventSubsystem;

class FHeartCoreInputPreprocessor : public IInputProcessor
{
public:
	FHeartCoreInputPreprocessor(UHeartInputEventSubsystem& Subsystem)
	  : InputSubsystem(Subsystem) {}

	virtual ~FHeartCoreInputPreprocessor() override {}

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {}

	/** Key down input */
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return false; }

	/** Key up input */
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return false; }

	/** Analog axis input */
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override { return false; }

	/** Mouse movement input */
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return false; }

	/** Mouse button press */
	virtual bool HandleMouseButtonDownEvent( FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

	/** Mouse button release */
	virtual bool HandleMouseButtonUpEvent( FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

	/** Mouse button double clicked. */
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return false; }

	/** Mouse wheel input */
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override { return false; }

	/** Called when a motion-driven device has new input */
	//virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) override { return false; };

	/** Debug name for logging purposes */
	virtual const TCHAR* GetDebugName() const override { return TEXT("HeartCore"); }

private:
	UHeartInputEventSubsystem& InputSubsystem;
};
