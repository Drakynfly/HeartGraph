// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputActivation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputActivation)

EHeartInputActivationType UHeartInputActivationLibrary::GetActivationType(const FHeartInputActivation& Activation)
{
	auto&& Struct = Activation.GetScriptStruct();
	if (Struct == FKeyEvent::StaticStruct()) return EHeartInputActivationType::KeyEvent;
	if (Struct == FPointerEvent::StaticStruct()) return EHeartInputActivationType::PointerEvent;
	if (Struct == FHeartInputKeyParams::StaticStruct()) return EHeartInputActivationType::InputKeyParams;
	if (Struct == FHeartManualEvent::StaticStruct()) return EHeartInputActivationType::Manual;
	if (Struct == FHeartActionIsRedo::StaticStruct()) return EHeartInputActivationType::Redo;
	return EHeartInputActivationType::Invalid;
}

EHeartInputActivationType UHeartInputActivationLibrary::SwitchOnActivationType(
	const FHeartInputActivation& Activation)
{
	return GetActivationType(Activation);
}

bool UHeartInputActivationLibrary::IsRedoAction(const FHeartInputActivation& Activation)
{
	return Activation.GetScriptStruct() == FHeartActionIsRedo::StaticStruct();
}

FHeartManualEvent UHeartInputActivationLibrary::ActivationToManualEvent(const FHeartInputActivation& Activation)
{
	if (TOptional<FHeartManualEvent> ManualEvent = Activation.As<FHeartManualEvent>();
		ManualEvent.IsSet())
	{
		return ManualEvent.GetValue();
	}
	return FHeartManualEvent();
}

FKeyEvent UHeartInputActivationLibrary::ActivationToKeyEvent(const FHeartInputActivation& Activation)
{
	if (TOptional<FKeyEvent> KeyEvent = Activation.As<FKeyEvent>();
		KeyEvent.IsSet())
	{
		return KeyEvent.GetValue();
	}
	return FKeyEvent();
}

FPointerEvent UHeartInputActivationLibrary::ActivationToPointerEvent(const FHeartInputActivation& Activation)
{
	if (TOptional<FPointerEvent> PointerEvent = Activation.As<FPointerEvent>();
		PointerEvent.IsSet())
	{
		return PointerEvent.GetValue();
	}
	return FPointerEvent();
}

FHeartInputKeyParams UHeartInputActivationLibrary::ActivationToInputKeyParams(const FHeartInputActivation& Activation)
{
	if (TOptional<FInputKeyParams> InputKeyParams = Activation.As<FInputKeyParams>();
		InputKeyParams.IsSet())
	{
		return InputKeyParams.GetValue();
	}
	return FHeartInputKeyParams();
}