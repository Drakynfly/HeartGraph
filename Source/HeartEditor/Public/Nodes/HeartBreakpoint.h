// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartBreakpoint.generated.h"

// @todo revisit breakpoint system. it isn't really even being used. either remove, or expand usability

USTRUCT()
struct HEARTEDITOR_API FHeartBreakpoint
{
	GENERATED_BODY()

	FHeartBreakpoint()
	{
		bHasBreakpoint = false;
		bBreakpointEnabled = false;
		bBreakpointHit = false;
	}

	UPROPERTY()
	bool bHasBreakpoint;

	bool bBreakpointEnabled;
	bool bBreakpointHit;

	void AddBreakpoint();
	void RemoveBreakpoint();
	bool HasBreakpoint() const;

	void EnableBreakpoint();
	bool CanEnableBreakpoint() const;

	void DisableBreakpoint();
	bool IsBreakpointEnabled() const;

	void ToggleBreakpoint();
};