// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/LocationModifiers/HeartNodeLocationModifier_SnapToGrid.h"

FVector2D UHeartNodeLocationModifier_SnapToGrid::LocationToProxy(const FVector2D& Location) const
{
	return FVector2D(Snap(Location.X, GridX), Snap(Location.Y, GridY));
}

FVector2D UHeartNodeLocationModifier_SnapToGrid::ProxyToLocation(const FVector2D& Proxy) const
{
	// Snapping is destructive. Original location is lost, just return proxy.
	return Proxy;
}

FVector UHeartNodeLocationModifier_SnapToGrid::LocationToProxy3D(const FVector& Location) const
{
	return Super::LocationToProxy3D(Location);
}

FVector UHeartNodeLocationModifier_SnapToGrid::ProxyToLocation3D(const FVector& Proxy) const
{
	// Snapping is destructive. Original location is lost, just return proxy.
	return Proxy;
}
