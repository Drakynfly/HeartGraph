// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartGraphNode3D.h"

FVector UHeartGraphNode3D::GetLocation3D() const
{
	return FVector(Location, Height);
}

void UHeartGraphNode3D::SetLocation3D(const FVector& NewLocation)
{
	SetLocation(FVector2D(NewLocation));
	Height = NewLocation.Z;
}
