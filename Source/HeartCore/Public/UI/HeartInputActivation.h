// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InputCoreTypes.h"
#include "InstancedStruct.h"
#include "HeartInputActivation.generated.h"

USTRUCT(BlueprintType)
struct FHeartManualEvent
{
	GENERATED_BODY()

	FHeartManualEvent() {}
	FHeartManualEvent(const double Value)
	  : EventValue(Value) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ManualEvent")
	double EventValue = 0.0;
};

/**
 *
 */
USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct HEARTCORE_API FHeartInputActivation
{
	GENERATED_BODY()

	FHeartInputActivation()
	  : EventStruct(nullptr) {}

	FHeartInputActivation(const FHeartManualEvent& ManualEvent)
	{
		EventStruct.InitializeAs<FHeartManualEvent>(ManualEvent);
	}

	FHeartInputActivation(const FKeyEvent& KeyEvent)
	{
		EventStruct.InitializeAs<FKeyEvent>(KeyEvent);
	}

	FHeartInputActivation(const FPointerEvent& PointerEvent)
	{
		EventStruct.InitializeAs<FPointerEvent>(PointerEvent);
	}

	FHeartManualEvent AsManualEvent() const
	{
		if (EventStruct.GetScriptStruct() == FHeartManualEvent::StaticStruct())
		{
			return EventStruct.Get<FHeartManualEvent>();
		}
		return FHeartManualEvent();
	}

	FKeyEvent AsKeyEvent() const
	{
		if (EventStruct.GetScriptStruct() == FKeyEvent::StaticStruct())
		{
			return EventStruct.Get<FKeyEvent>();
		}
		return FKeyEvent();
	}

	FPointerEvent AsPointerEvent() const
	{
		if (EventStruct.GetScriptStruct() == FPointerEvent::StaticStruct())
		{
			return EventStruct.Get<FPointerEvent>();
		}
		return FPointerEvent();
	}

	UPROPERTY()
	FInstancedStruct EventStruct;
};

UENUM(BlueprintType)
enum class EHeartInputActivationType : uint8
{
	// This Activation was not triggered correctly
	Invalid,

	// This Activation was triggered manually by code and has no associated InputEvent
	Manual,

	// This Activation was triggered by a KeyEvent
	KeyEvent,

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
	static FHeartManualEvent ActivationToManualEvent(const FHeartInputActivation& Activation);

	UFUNCTION(BlueprintPure, Category = "Heart|InputActivation")
	static FKeyEvent ActivationToKeyEvent(const FHeartInputActivation& Activation);

	UFUNCTION(BlueprintPure, Category = "Heart|InputActivation")
	static FPointerEvent ActivationToPointerEvent(const FHeartInputActivation& Activation);
};
