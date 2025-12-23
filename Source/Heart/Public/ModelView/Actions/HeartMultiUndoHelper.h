// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HAL/Platform.h"

class UHeartGraph;

// Utilities for creating a grouped set of actions that get undone/redone together
namespace Heart::Action::History::MultiUndo
{
	HEART_API void Start(UHeartGraph& Graph);
	HEART_API void End(UHeartGraph& Graph);
}