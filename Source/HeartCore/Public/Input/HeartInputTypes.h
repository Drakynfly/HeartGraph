// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

struct FHeartEvent;
struct FHeartInputActivation;

class UHeartInputHandlerAssetBase;

namespace Heart::Input
{
	template <typename T>
	struct TLinkerType
	{
		static constexpr bool Supported = false;
	};

	enum EExecutionOrder
	{
		None, // Blank layer. Do not use.
		Event, // Default layer. Handlers can capture input or bubble it.
		Deferred, // Layer for events that handle event in the moment, but continue to have effects for multiple frames.
		Listener, // Interception layer. Handlers can intercept, but cannot stop it from bubbling.

		HighestHandlingPriority = Deferred
	};

	struct FSortableCallback
	{
		TObjectPtr<const UHeartInputHandlerAssetBase> Handler;

		// Determines the order that callback handler run in, and whether they bubble the input callstack
		const EExecutionOrder Priority = None;

		friend bool operator<(const FSortableCallback& A, const FSortableCallback& B)
		{
			// Sort in reverse. Higher priorities should be ordered first, lower after.
			return A.Priority > B.Priority;
		}
	};
}