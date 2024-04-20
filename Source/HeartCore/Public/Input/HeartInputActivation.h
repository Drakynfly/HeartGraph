// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InputCoreTypes.h"
#include "InstancedStruct.h"
#include "Concepts/BaseStructureProvider.h"
#include "GameFramework/PlayerInput.h"
#include "HeartInputActivation.generated.h"

struct FBloodContainer;

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

	// When an action is being 're-done', this is the data originally used to first 'do', then 'undo' the action.
	FBloodContainer* UndoneData = nullptr;
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
		if constexpr (std::is_same_v<T, FInputKeyParams> ||
					  std::is_same_v<T, FHeartInputKeyParams>)
		{
			if (EventStruct.GetScriptStruct() == FHeartInputKeyParams::StaticStruct())
			{
				return EventStruct.Get<FHeartInputKeyParams>().Params;
			}
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

	template <
		typename T
		UE_REQUIRES(TIsHeartInputActivationType<T>::Value)
	>
	T AsOrDefault() const
	{
		if (TOptional<T> Option = As<T>();
			Option.IsSet())
		{
			return Option.GetValue();
		}
		return T();
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