// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InputCoreTypes.h"
#include "InstancedStruct.h"
#include "Concepts/BaseStructureProvider.h"
#include "GameFramework/PlayerInput.h"
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

// Wrapper around FInputKeyParams
USTRUCT(BlueprintType)
struct FHeartInputKeyParams
{
	GENERATED_BODY()

	FHeartInputKeyParams() {}
	FHeartInputKeyParams(const FInputKeyParams& Params)
	  : Params(Params) {}

	FInputKeyParams Params;
};

// Flag struct to mark an InputActivation as a redo
USTRUCT()
struct FHeartActionIsRedo
{
	GENERATED_BODY()
};

template <typename T>
struct TIsHeartInputActivationType
{
	static constexpr bool Value = false;
};
template <> struct TIsHeartInputActivationType<FHeartManualEvent>	{ static constexpr bool Value = true; };
template <> struct TIsHeartInputActivationType<FKeyEvent>			{ static constexpr bool Value = true; };
template <> struct TIsHeartInputActivationType<FPointerEvent>		{ static constexpr bool Value = true; };
template <> struct TIsHeartInputActivationType<FInputKeyParams>		{ static constexpr bool Value = true; };
template <> struct TIsHeartInputActivationType<FHeartActionIsRedo>	{ static constexpr bool Value = true; };

/**
 *
 */
USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct HEARTCORE_API FHeartInputActivation
{
	GENERATED_BODY()

	FHeartInputActivation()
	  : EventStruct(nullptr) {}

	template <
		typename T
		UE_REQUIRES(TIsHeartInputActivationType<T>::Value)
	>
	FHeartInputActivation(const T& Type)
	{
		if constexpr (std::is_same_v<T, FInputKeyParams>)
		{
			EventStruct.InitializeAs<FHeartInputKeyParams>(Type);
		}
		else
		{
			EventStruct.InitializeAs<T>(Type);
		}
	}

	template <
		typename T
		UE_REQUIRES(TIsHeartInputActivationType<T>::Value)
	>
	TOptional<T> As() const
	{
		if constexpr (std::is_same_v<T, FInputKeyParams>)
		{
			return EventStruct.Get<FHeartInputKeyParams>().Params;
		}

		if constexpr (TModels_V<CBaseStructureProvider, T>)
		{
			if (EventStruct.GetScriptStruct() == TBaseStructure<T>::Get())
			{
				return EventStruct.Get<T>();
			}
		}
		return {};
	}

	bool IsRedoAction() const
	{
		return EventStruct.GetScriptStruct() == FHeartActionIsRedo::StaticStruct();
	}

	const UScriptStruct* GetScriptStruct() const { return EventStruct.GetScriptStruct(); }

	friend FArchive& operator<<(FArchive& Ar, FHeartInputActivation& Activation)
	{
		Activation.EventStruct.Serialize(Ar);
		return Ar;
	}

private:
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