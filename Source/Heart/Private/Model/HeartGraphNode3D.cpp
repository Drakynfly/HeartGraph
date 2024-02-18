// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Model/HeartGraphNode3D.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphNode3D)

FVector UHeartGraphNode3D::GetLocation3D() const
{
	return FVector(Location, Height);
}

void UHeartGraphNode3D::SetLocation3D(const FVector& NewLocation)
{
	SetLocation(FVector2D(NewLocation));
	Height = NewLocation.Z;
	OnNodeLocation3DChanged.Broadcast(this, FVector(Location, Height));
}