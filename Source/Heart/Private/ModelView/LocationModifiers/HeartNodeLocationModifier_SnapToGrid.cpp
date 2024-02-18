// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/LocationModifiers/HeartNodeLocationModifier_SnapToGrid.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNodeLocationModifier_SnapToGrid)

template <typename T>
static T Snap(T Value, T Grid)
{
	return Grid * FMath::RoundHalfToEven(Value / Grid);
}

FVector2D UHeartNodeLocationModifier_SnapToGrid::LocationToProxy(const FVector2D& Location) const
{
	FVector2D Proxy;
	Proxy.X = SnapOnX ? Snap(Location.X, GridX) : Location.X;
	Proxy.Y = SnapOnY ? Snap(Location.Y, GridY) : Location.Y;
	return Proxy;
}

FVector2D UHeartNodeLocationModifier_SnapToGrid::ProxyToLocation(const FVector2D& Proxy) const
{
	// Snapping is destructive. Original location is lost, just return proxy.
	return Proxy;
}

FVector UHeartNodeLocationModifier_SnapToGrid::LocationToProxy3D(const FVector& Location) const
{
	FVector Proxy;
	Proxy.X = SnapOnX ? Snap(Location.X, GridX) : Location.X;
	Proxy.Y = SnapOnY ? Snap(Location.Y, GridY) : Location.Y;
	Proxy.Z = SnapOnZ ? Snap(Location.Z, GridZ) : Location.Z;
	return Proxy;}

FVector UHeartNodeLocationModifier_SnapToGrid::ProxyToLocation3D(const FVector& Proxy) const
{
	// Snapping is destructive. Original location is lost, just return proxy.
	return Proxy;
}