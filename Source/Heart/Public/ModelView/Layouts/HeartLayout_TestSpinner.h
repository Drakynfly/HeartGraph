// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ModelView/HeartLayoutHelper.h"
#include "HeartLayout_TestSpinner.generated.h"

/**
 * This just spins nodes around. No real point, it's just to test layout ticking.
 */
UCLASS()
class HEART_API UHeartLayout_TestSpinner : public UHeartLayoutHelper
{
	GENERATED_BODY()

public:
	virtual bool Layout(IHeartGraphInterface* Interface, const TArray<FHeartNodeGuid>& Nodes, float DeltaTime) override;
};