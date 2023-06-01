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

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override { return false; }
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return false; }
	virtual bool HandleMouseButtonDownEvent( FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonUpEvent( FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return false; }
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override { return false; }

	/** Called when a motion-driven device has new input */
	//virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) override { return false; };

	/** Debug name for logging purposes */
	virtual const TCHAR* GetDebugName() const override { return TEXT("HeartCore"); }

private:
	UHeartInputEventSubsystem& InputSubsystem;
};
