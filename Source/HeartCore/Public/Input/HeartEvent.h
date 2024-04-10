// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "Concepts/BaseStructureProvider.h"
#include "HeartEvent.generated.h"

UINTERFACE(MinimalAPI)
class UHeartDeferredEventHandler : public UInterface
{
	GENERATED_BODY()
};

class HEARTCORE_API IHeartDeferredEventHandler
{
	GENERATED_BODY()

	// This interface doesn't have any functions. Its only use is to earmark classes that defer handling.
};

USTRUCT()
struct FHeartDeferredEvent
{
	GENERATED_BODY()

	FHeartDeferredEvent() {}

	FHeartDeferredEvent(IHeartDeferredEventHandler* Handler)
	  : Handler(Cast<UObject>(Handler)) {}

	FHeartDeferredEvent(const TScriptInterface<IHeartDeferredEventHandler> Handler)
	  : Handler(Handler) {}

	UPROPERTY()
	TScriptInterface<IHeartDeferredEventHandler> Handler;
};


template <typename T>
struct TIsHeartEventType
{
	static constexpr bool Value = false;
};

template <> struct TIsHeartEventType<FHeartDeferredEvent> { static constexpr bool Value = true; };

/**
 *
 */
USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct HEARTCORE_API FHeartEvent
{
	GENERATED_BODY()

	FHeartEvent()
	  : Status(-1) {}

private:
	FHeartEvent(const int32 StatusCode)
	 : Status(StatusCode) {}

public:
	template <
		typename T,
		typename... TArgs
		UE_REQUIRES(TIsHeartEventType<T>::Value)
	>
	FHeartEvent& Detail(TArgs... Args)
	{
		Details.InitializeAs<T>(Args...);
		return *this;
	}

	template <
		typename T,
		typename... TArgs
		UE_REQUIRES(TIsHeartEventType<T>::Value)
	>
	FHeartEvent Detail(TArgs... Args) const
	{
		FHeartEvent Event = *this;
		Event.Details.InitializeAs<T>(Args...);
		return Event;
	}

	template <
		typename T
		UE_REQUIRES(TIsHeartEventType<T>::Value)
	>
	TOptional<T> As() const
	{
		if constexpr (TModels_V<CBaseStructureProvider, T>)
		{
			if (Details.GetScriptStruct() == TBaseStructure<T>::Get())
			{
				return Details.Get<T>();
			}
		}
		return {};
	}

	const UScriptStruct* GetScriptStruct() const { return Details.GetScriptStruct(); }

	// Event that was invalid, and not handled
	static const FHeartEvent Invalid;

	// Event that was valid, but intentionally not captured
	static const FHeartEvent Ignored;

	// Event that was valid, and handled successfully
	static const FHeartEvent Handled;

	// Event that was valid, but handler failed to run for some reason
	static const FHeartEvent Failed;

	bool WasEventInvalid() const;
	bool WasEventIgnored() const;
	bool WasEventHandled() const;
	bool DidHandlingFail() const;

	// Ignored or Handled are both "successful" replies, in that everything was working as intended.
	bool WasEventSuccessful() const
	{
		return WasEventIgnored() || WasEventHandled();
	}

	// Handled or Failure are both capturing responses, and input should not be further bubbled.
	bool WasEventCaptured() const
	{
		return WasEventHandled() || DidHandlingFail();
	}

private:
	// Simple status code
	UPROPERTY()
	int32 Status;

	// Detailed response struct that can be interpreted by specific input linkers
	UPROPERTY()
	FInstancedStruct Details;
};


UCLASS()
class UHeartEventLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Event that was invalid, and not handled
	UFUNCTION(BlueprintPure, Category = "Heart|Event")
	static bool WasEventInvalid(const FHeartEvent& Event);

	// Event that was valid, but intentionally not captured
	UFUNCTION(BlueprintPure, Category = "Heart|Event")
	static bool WasEventIgnored(const FHeartEvent& Event);

	// Event that was valid, and handled successfully
	UFUNCTION(BlueprintPure, Category = "Heart|Event")
	static bool WasEventHandled(const FHeartEvent& Event);

	// Event that was valid, but handler failed to run for some reason
	UFUNCTION(BlueprintPure, Category = "Heart|Event")
	static bool DidHandlingFail(const FHeartEvent& Event);

	// Ignored or Handled are both "successful" replies, in that everything was working as intended.
	UFUNCTION(BlueprintPure, Category = "Heart|Event")
	static bool WasEventSuccessful(const FHeartEvent& Event);

	// Handled or Failure are both capturing responses, and input should not be further bubbled.
	UFUNCTION(BlueprintPure, Category = "Heart|Event")
	static bool WasEventCaptured(const FHeartEvent& Event);

	UFUNCTION(BlueprintPure, Category = "Heart|Event")
	static FHeartEvent MakeDeferredEvent(const FHeartEvent& Event, TScriptInterface<IHeartDeferredEventHandler> Handler);

	UFUNCTION(BlueprintPure, Category = "Heart|Event")
	static bool IsDeferredEvent(const FHeartEvent& Event);
};