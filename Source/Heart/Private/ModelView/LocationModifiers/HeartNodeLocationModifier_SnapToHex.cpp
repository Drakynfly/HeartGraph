// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/LocationModifiers/HeartNodeLocationModifier_SnapToHex.h"
#include "General/HeartHex.h"

FVector2D UHeartNodeLocationModifier_SnapToHex::LocationToProxy(const FVector2D& Location) const
{
	return Heart::Hex::SnapToNearestHex(Location, GridSize);
}

FVector2D UHeartNodeLocationModifier_SnapToHex::ProxyToLocation(const FVector2D& Proxy) const
{
	// Snapping is destructive. Original location is lost, just return proxy.
	return Proxy;
}

FVector UHeartNodeLocationModifier_SnapToHex::LocationToProxy3D(const FVector& Location) const
{
	return Super::LocationToProxy3D(Location);
}

FVector UHeartNodeLocationModifier_SnapToHex::ProxyToLocation3D(const FVector& Proxy) const
{
	// Snapping is destructive. Original location is lost, just return proxy.
	return Proxy;
}
