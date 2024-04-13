// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartInputTypes.generated.h"

struct FHeartEvent;
struct FHeartInputActivation;

class UHeartInputHandlerAssetBase;

template <typename T>
struct THeartInputLinkerType
{
	static constexpr bool Supported = false;
};

enum class EHeartInputExecutionOrder
{
	None, // Blank layer. Do not use.
	Event, // Default layer. Handlers can capture input or bubble it.
	Deferred, // Layer for events that handle event at the moment, but continue to have effects for multiple frames.
	Listener, // Interception layer. Handlers can intercept, but cannot stop it from bubbling.

	HighestHandlingPriority = Deferred
};

USTRUCT()
struct FHeartSortableInputCallback
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<const UHeartInputHandlerAssetBase> Handler;

	// Determines the order that callback handler runs in, and whether they bubble the input callstack
	EHeartInputExecutionOrder Priority = EHeartInputExecutionOrder::None;
};

USTRUCT()
struct FHeartSortableCallbackList
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FHeartSortableInputCallback> Callbacks;
};