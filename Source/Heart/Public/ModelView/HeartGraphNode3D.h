// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphNode.h"
#include "HeartGraphNode3D.generated.h"

/**
 * Base class for nodes that are represented in 3 dimensions.
 */
UCLASS(Abstract)
class HEART_API UHeartGraphNode3D : public UHeartGraphNode
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	FVector GetLocation3D() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	void SetLocation3D(const FVector& NewLocation);

private:
	UPROPERTY()
	float Height;
};
