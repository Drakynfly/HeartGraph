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

namespace Heart
{
	template <typename T>
	struct TIsInputActivationType
	{
		static constexpr bool Value = false;
	};

	// Defines which structs are considered "Activation Types"
	template <> struct TIsInputActivationType<FHeartManualEvent>	{ static constexpr bool Value = true; };
	template <> struct TIsInputActivationType<FKeyEvent>			{ static constexpr bool Value = true; };
	template <> struct TIsInputActivationType<FPointerEvent>		{ static constexpr bool Value = true; };
	template <> struct TIsInputActivationType<FInputKeyEventArgs>	{ static constexpr bool Value = true; };
	template <> struct TIsInputActivationType<FHeartActionIsRedo>	{ static constexpr bool Value = true; };

	template<typename T>
	concept CInputActivationType = TIsInputActivationType<T>::Value;
}

/**
 *
 */
USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct HEARTCORE_API FHeartInputActivation
{
	GENERATED_BODY()

	FHeartInputActivation()
	  : EventStruct(nullptr) {}

	template <Heart::CInputActivationType T>
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

	template <Heart::CInputActivationType T>
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

	template <Heart::CInputActivationType T>
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