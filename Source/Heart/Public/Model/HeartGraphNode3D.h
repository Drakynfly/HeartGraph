// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphNode.h"
#include "HeartGraphNode3D.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGraphNodeLocation3DChanged, class UHeartGraphNode3D*, Node, const FVector, Location);

/**
 * Base class for nodes that are represented in 3 dimensions.
 */
UCLASS(Abstract)
class HEART_API UHeartGraphNode3D : public UHeartGraphNode
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode3D")
	FVector GetLocation3D() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode3D")
	void SetLocation3D(const FVector& NewLocation);

	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FOnGraphNodeLocation3DChanged OnNodeLocation3DChanged;

private:
	UPROPERTY()
	double Height;
};
