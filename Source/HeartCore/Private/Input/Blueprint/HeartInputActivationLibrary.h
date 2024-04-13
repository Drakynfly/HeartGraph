// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Input/HeartInputActivation.h"
#include "HeartInputActivationLibrary.generated.h"

UENUM(BlueprintType)
enum class EHeartInputActivationType : uint8
{
	// This Activation was not triggered correctly
	Invalid,

	// This Activation was triggered manually by code and has no associated InputEvent
	Manual,

	// This Activation was triggered as a Redo of a previously undone action
	Redo,

	// This Activation was triggered by a KeyEvent
	KeyEvent,

	// This Activation was triggered by InputKeyParams
	InputKeyParams,

	// This Activation was triggered by a PointerEvent
	PointerEvent,
};

UCLASS()
class UHeartInputActivationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Heart|InputActivation")
	static EHeartInputActivationType GetActivationType(const FHeartInputActivation& Activation);

	UFUNCTION(BlueprintCallable, Category = "Heart|InputActivation", meta = (ExpandEnumAsExecs = "ReturnValue"))
	static EHeartInputActivationType SwitchOnActivationType(const FHeartInputActivation& Activation);

	UFUNCTION(BlueprintPure, Category = "Heart|InputActivation")
	static bool IsRedoAction(const FHeartInputActivation& Activation);

	UFUNCTION(BlueprintPure, Category = "Heart|InputActivation")
	static FHeartManualEvent ActivationToManualEvent(const FHeartInputActivation& Activation);

	UFUNCTION(BlueprintPure, Category = "Heart|InputActivation")
	static FKeyEvent ActivationToKeyEvent(const FHeartInputActivation& Activation);

	UFUNCTION(BlueprintPure, Category = "Heart|InputActivation")
	static FPointerEvent ActivationToPointerEvent(const FHeartInputActivation& Activation);

	UFUNCTION(BlueprintPure, Category = "Heart|InputActivation")
	static FHeartInputKeyParams ActivationToInputKeyParams(const FHeartInputActivation& Activation);
};