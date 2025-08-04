// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HAL/Platform.h"

class IHeartGraphInterface;

// Utilities for creating a grouped set of actions that get undone/redone together
namespace Heart::Action::History::MultiUndo
{
	HEART_API void Start(IHeartGraphInterface* Graph);
	HEART_API void End(IHeartGraphInterface* Graph);
}