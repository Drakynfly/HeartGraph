// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartInputActivationLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputActivationLibrary)

EHeartInputActivationType UHeartInputActivationLibrary::GetActivationType(const FHeartInputActivation& Activation)
{
	auto&& Struct = Activation.GetScriptStruct();
	if (Struct == FKeyEvent::StaticStruct()) return EHeartInputActivationType::KeyEvent;
	if (Struct == FPointerEvent::StaticStruct()) return EHeartInputActivationType::PointerEvent;
	if (Struct == FHeartInputKeyEventArgs::StaticStruct()) return EHeartInputActivationType::InputKeyEventArgs;
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
	return Activation.IsRedoAction();
}

FHeartManualEvent UHeartInputActivationLibrary::ActivationToManualEvent(const FHeartInputActivation& Activation)
{
	return Activation.AsOrDefault<FHeartManualEvent>();
}

FKeyEvent UHeartInputActivationLibrary::ActivationToKeyEvent(const FHeartInputActivation& Activation)
{
	return Activation.AsOrDefault<FKeyEvent>();
}

FPointerEvent UHeartInputActivationLibrary::ActivationToPointerEvent(const FHeartInputActivation& Activation)
{
	return Activation.AsOrDefault<FPointerEvent>();
}

FHeartInputKeyEventArgs UHeartInputActivationLibrary::ActivationToInputKeyParams(const FHeartInputActivation& Activation)
{
	return Activation.AsOrDefault<FInputKeyEventArgs>();
}