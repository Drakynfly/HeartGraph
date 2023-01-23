// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartLayoutHelper.generated.h"

class IHeartNodeLocationAccessor;
class UHeartGraphNode;

/**
 *
 */
UCLASS(Abstract, const)
class HEART_API UHeartLayoutHelper : public UObject
{
	GENERATED_BODY()

public:
	// Required to override to implement logic.
	virtual void Layout(IHeartNodeLocationAccessor* Accessor, const TArray<UHeartGraphNode*>& Nodes)
		PURE_VIRTUAL(UHeartLayoutHelper::Layout, )

	// Overload that calls Layout on all Nodes in the Graph.
	void Layout(IHeartNodeLocationAccessor* Accessor);
};
