// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InputKeyEventArgs.h"
#include "StructUtils/InstancedStruct.h"
#include "Concepts/BaseStructureProvider.h"
#include "HeartInputActivation.generated.h"

struct FBloodContainer;
struct FKeyEvent;
struct FPointerEvent;

USTRUCT(BlueprintType)
struct FHeartManualEvent
{
	GENERATED_BODY()

	FHeartManualEvent() = default;
	FHeartManualEvent(const double Value)
	  : EventValue(Value) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ManualEvent")
	double EventValue = 0.0;
};

// Wrapper around FInputKeyEventArgs
USTRUCT(BlueprintType)
struct FHeartInputKeyEventArgs
{
	GENERATED_BODY()

	FHeartInputKeyEventArgs() = default;
	FHeartInputKeyEventArgs(const FInputKeyEventArgs& Args)
	  : Args(Args) {}

	FInputKeyEventArgs Args;
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
template <> struct TIsHeartInputActivationType<FInputKeyEventArgs>	{ static constexpr bool Value = true; };
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
		if constexpr (std::is_same_v<T, FInputKeyEventArgs>)
		{
			EventStruct.InitializeAs<FHeartInputKeyEventArgs>(Type);
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
		if constexpr (std::is_same_v<T, FInputKeyEventArgs> ||
					  std::is_same_v<T, FHeartInputKeyEventArgs>)
		{
			if (EventStruct.GetScriptStruct() == FHeartInputKeyEventArgs::StaticStruct())
			{
				return EventStruct.Get<FHeartInputKeyEventArgs>().Args;
			}
		}

		if constexpr (TModels_V<CBaseStructureProvider, T>)
		{
			if (EventStruct.GetScriptStruct() == TBaseStructure<T>::Get())
			{
				return EventStruct.Get<T>();
			}
		}
		return NullOpt;
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