// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartNodeLocationModifier.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNode3D.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNodeLocationModifier)

FVector2D UHeartNodeLocationModifierStack::LocationToProxy(const FVector2D& Location) const
{
	FVector2D Proxy = Location;

	for (int32 i = 0; i < Modifiers.Num(); ++i)
	{
		Proxy = Modifiers[i]->LocationToProxy(Proxy);
	}

	return Proxy;
}

FVector2D UHeartNodeLocationModifierStack::ProxyToLocation(const FVector2D& Proxy) const
{
	FVector2D Location = Proxy;

	for (int32 i = Modifiers.Num() - 1; i >= 0; --i)
	{
		Location = Modifiers[i]->ProxyToLocation(Location);
	}

	return Location;
}

FVector UHeartNodeLocationModifierStack::LocationToProxy3D(const FVector& Location) const
{
	FVector Proxy = Location;

	for (int32 i = 0; i < Modifiers.Num(); ++i)
	{
		Proxy = Modifiers[i]->LocationToProxy3D(Proxy);
	}

	return Proxy;
}

FVector UHeartNodeLocationModifierStack::ProxyToLocation3D(const FVector& Proxy) const
{
	FVector Location = Proxy;

	for (int32 i = Modifiers.Num() - 1; i >= 0; --i)
	{
		Location = Modifiers[i]->ProxyToLocation3D(Location);
	}

	return Location;
}