// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/LocationModifiers/HeartNodeLocationModifier_CartesianToPolar.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNodeLocationModifier_CartesianToPolar)

FVector2D UHeartNodeLocationModifier_CartesianToPolar::LocationToProxy(const FVector2D& Location) const
{
	double Radius;
	double Angle;
	FMath::CartesianToPolar(Location.X, Location.Y, Radius, Angle);
	return FVector2D::UnitX().GetRotated(Angle) * Radius;
}

FVector2D UHeartNodeLocationModifier_CartesianToPolar::ProxyToLocation(const FVector2D& Proxy) const
{
	const double Radius = Proxy.Length();
	const double Angle = FMath::RadiansToDegrees(FMath::Atan2(Proxy.Y, Proxy.X));
	FVector2D OutCartesian;
	FMath::PolarToCartesian(Radius, Angle, OutCartesian.X, OutCartesian.Y);
	return OutCartesian;
}

FVector UHeartNodeLocationModifier_CartesianToPolar::LocationToProxy3D(const FVector& Location) const
{
	double Radius;
	double Angle;
	FMath::CartesianToPolar(Location.X, Location.Y, Radius, Angle);
	return FVector::ForwardVector.RotateAngleAxis(Angle, FVector::UpVector) * Radius;
}

FVector UHeartNodeLocationModifier_CartesianToPolar::ProxyToLocation3D(const FVector& Proxy) const
{
	const double Radius = Proxy.Length();
	const double Angle = FMath::RadiansToDegrees(FMath::Atan2(Proxy.Y, Proxy.X));
	FVector OutCartesian;
	FMath::PolarToCartesian(Radius, Angle, OutCartesian.X, OutCartesian.Y);
	OutCartesian.Z = Proxy.Z;
	return OutCartesian;
}