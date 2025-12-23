// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartNodeIndex.generated.h"

/**
 * A simple int wrapper to type-safely index into the Nodes map on a graph.
 * Unlike FHeartNodeGuid, this is not stable, and should only be used for communication during a single frame.
 */
USTRUCT()
struct FHeartNodeIndex
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Index = INDEX_NONE;
};
