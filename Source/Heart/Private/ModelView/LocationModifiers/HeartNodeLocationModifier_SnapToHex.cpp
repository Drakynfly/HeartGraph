// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/LocationModifiers/HeartNodeLocationModifier_SnapToHex.h"
#include "General/HeartHex.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNodeLocationModifier_SnapToHex)

FVector2D UHeartNodeLocationModifier_SnapToHex::LocationToProxy(const FVector2D& Location) const
{
	switch (Orientation)
	{
	default:
	case EHeartHexOrientation::FlatTop:
		return Heart::Hex::SnapToNearestHex_Flat(Location, GridSize);
	case EHeartHexOrientation::PointyTop:
		return Heart::Hex::SnapToNearestHex_Pointy(Location, GridSize);
	}
}

FVector2D UHeartNodeLocationModifier_SnapToHex::ProxyToLocation(const FVector2D& Proxy) const
{
	// Snapping is destructive. Original location is lost, just return proxy.
	return Proxy;
}

FVector UHeartNodeLocationModifier_SnapToHex::LocationToProxy3D(const FVector& Location) const
{
	switch (Orientation)
	{
	default:
	case EHeartHexOrientation::FlatTop:
		return FVector(Heart::Hex::SnapToNearestHex_Flat(FVector2D(Location.X, Location.Y), GridSize), Location.Z);
	case EHeartHexOrientation::PointyTop:
		return FVector(Heart::Hex::SnapToNearestHex_Pointy(FVector2D(Location.X, Location.Y), GridSize), Location.Z);
	}
}

FVector UHeartNodeLocationModifier_SnapToHex::ProxyToLocation3D(const FVector& Proxy) const
{
	// Snapping is destructive. Original location is lost, just return proxy.
	return Proxy;
}