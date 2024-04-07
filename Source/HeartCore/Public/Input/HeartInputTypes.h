// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

struct FHeartEvent;
struct FHeartInputActivation;

namespace Heart::Input
{
	template <typename T>
	struct TLinkerType
	{
		static constexpr bool Supported = false;
	};

	using FHandlerDelegate = TDelegate<FHeartEvent(UObject*, const FHeartInputActivation&)>;
	using FDescriptionDelegate = TDelegate<FText(const UObject*)>;
	using FConditionDelegate = TDelegate<bool(const UObject*)>;

	enum EExecutionOrder
	{
		None, // Blank layer. Do not use.
		Event, // Default layer. Handlers can capture input or bubble it.
		Deferred, // Layer for events that handle event in the moment, but continue to have effects for multiple frames.
		Listener, // Interception layer. Handlers can intercept, but cannot stop it from bubbling.

		HighestHandlingPriority = Deferred
	};

	struct FConditionalCallback
	{
		// Callback to execute the event
		const FHandlerDelegate Handler;

		// Callback to retrieve a text description of the action
		const FDescriptionDelegate Description;

		// Callback to determine if the context of the trigger is valid for executing the action
		const FConditionDelegate Condition;

		// Determines the order that callback handler run in, and whether they bubble the input callstack
		const EExecutionOrder Priority = None;

		friend bool operator<(const FConditionalCallback& A, const FConditionalCallback& B)
		{
			// Sort in reverse. Higher priorities should be ordered first, lower after.
			return A.Priority > B.Priority;
		}
	};
}