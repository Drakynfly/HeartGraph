// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ModelView/HeartNodeLocationAccessor.h"
#include "HeartNodeLocationModifier_CartesianToPolar.generated.h"

/**
 * Convert cartesian locations to polar
 */
UCLASS(DisplayName = "Modifier - Cartesian to Polar")
class HEART_API UHeartNodeLocationModifier_CartesianToPolar : public UHeartNodeLocationModifier
{
	GENERATED_BODY()

protected:
	virtual FVector2D LocationToProxy(const FVector2D& Location) const override;
	virtual FVector2D ProxyToLocation(const FVector2D& Proxy) const override;

	virtual FVector LocationToProxy3D(const FVector& Location) const override;
	virtual FVector ProxyToLocation3D(const FVector& Proxy) const override;
};
