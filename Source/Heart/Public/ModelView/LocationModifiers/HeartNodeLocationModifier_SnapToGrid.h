// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ModelView/HeartNodeLocationAccessor.h"
#include "HeartNodeLocationModifier_SnapToGrid.generated.h"

/**
 * Rounds locations to snap to a grid
 */
UCLASS(DisplayName = "Modifier - Snap to Grid")
class HEART_API UHeartNodeLocationModifier_SnapToGrid : public UHeartNodeLocationModifier
{
	GENERATED_BODY()

public:
	virtual FVector2D LocationToProxy(const FVector2D& Location) const override;
	virtual FVector2D ProxyToLocation(const FVector2D& Proxy) const override;

	virtual FVector LocationToProxy3D(const FVector& Location) const override;
	virtual FVector ProxyToLocation3D(const FVector& Proxy) const override;

protected:
	template <typename T>
	static T Snap(T Value, T Grid)
	{
		return Grid * FMath::RoundHalfToEven(Value / Grid);
	}

	UPROPERTY(EditAnywhere, Category = "SnapToGrid")
	double GridX = 1.0;

	UPROPERTY(EditAnywhere, Category = "SnapToGrid")
	double GridY = 1.0;

	UPROPERTY(EditAnywhere, Category = "SnapToGrid")
	double GridZ = 1.0;
};
