// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartInputActivation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputActivation)

EHeartInputActivationType UHeartInputActivationLibrary::GetActivationType(const FHeartInputActivation& Activation)
{
	auto&& Struct = Activation.EventStruct.GetScriptStruct();
	if (Struct == FKeyEvent::StaticStruct()) return EHeartInputActivationType::KeyEvent;
	if (Struct == FPointerEvent::StaticStruct()) return EHeartInputActivationType::PointerEvent;
	if (Struct == FHeartManualEvent::StaticStruct()) return EHeartInputActivationType::Manual;
	return EHeartInputActivationType::Invalid;
}

EHeartInputActivationType UHeartInputActivationLibrary::SwitchOnActivationType(
	const FHeartInputActivation& Activation)
{
	return GetActivationType(Activation);
}

FHeartManualEvent UHeartInputActivationLibrary::ActivationToManualEvent(const FHeartInputActivation& Activation)
{
	return Activation.AsManualEvent();
}

FKeyEvent UHeartInputActivationLibrary::ActivationToKeyEvent(const FHeartInputActivation& Activation)
{
	return Activation.AsKeyEvent();
}

FPointerEvent UHeartInputActivationLibrary::ActivationToPointerEvent(const FHeartInputActivation& Activation)
{
	return Activation.AsPointerEvent();
}