// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphNode.h"
#include "HeartGraphNode3D.generated.h"

/**
 * Base class for nodes that are represented in 3 dimensions.
 */
UCLASS(Abstract)
class UE_DEPRECATED(5.7, "3D Nodes no longer exist. 3D Locations are stored in the Node Location Component")
UHeartGraphNode3D : public UHeartGraphNode
{
	GENERATED_BODY()
};
