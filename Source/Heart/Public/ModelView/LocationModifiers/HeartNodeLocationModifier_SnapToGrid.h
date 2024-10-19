// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ModelView/HeartNodeLocationModifier.h"
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
	UPROPERTY(EditAnywhere, Category = "SnapToGrid", meta = (InlineEditConditionToggle))
	bool SnapOnX = false;

	UPROPERTY(EditAnywhere, Category = "SnapToGrid", meta = (InlineEditConditionToggle))
	bool SnapOnY = false;

	UPROPERTY(EditAnywhere, Category = "SnapToGrid", meta = (InlineEditConditionToggle))
	bool SnapOnZ = false;

	UPROPERTY(EditAnywhere, Category = "SnapToGrid", meta = (EditCondition = "SnapOnX", ClampMin = 0.01, UIMin = 1.0))
	double GridX = 1.0;

	UPROPERTY(EditAnywhere, Category = "SnapToGrid", meta = (EditCondition = "SnapOnY", ClampMin = 0.01, UIMin = 1.0))
	double GridY = 1.0;

	UPROPERTY(EditAnywhere, Category = "SnapToGrid", meta = (EditCondition = "SnapOnZ", ClampMin = 0.01, UIMin = 1.0))
	double GridZ = 1.0;
};
