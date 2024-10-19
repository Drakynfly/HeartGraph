// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Model/HeartGraphInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartNodeLocationModifier.generated.h"


UCLASS(Abstract, const, EditInlineNew, CollapseCategories)
class HEART_API UHeartNodeLocationModifier : public UObject
{
	GENERATED_BODY()

public:
	virtual FVector2D LocationToProxy(const FVector2D& Location) const PURE_VIRTUAL(UHeartNodeLocationProxyLayer::LocationToProxy, return FVector2D(); )
	virtual FVector2D ProxyToLocation(const FVector2D& Proxy) const PURE_VIRTUAL(UHeartNodeLocationProxyLayer::ProxyToLocation, return FVector2D(); )

	virtual FVector LocationToProxy3D(const FVector& Location) const PURE_VIRTUAL(UHeartNodeLocationProxyLayer::LocationToProxy3D, return FVector(); )
	virtual FVector ProxyToLocation3D(const FVector& Proxy) const PURE_VIRTUAL(UHeartNodeLocationProxyLayer::ProxyToLocation3D, return FVector(); )
};


UCLASS(NotEditInlineNew, CollapseCategories)
class HEART_API UHeartNodeLocationModifierStack : public UHeartNodeLocationModifier
{
	GENERATED_BODY()

public:
	virtual FVector2D LocationToProxy(const FVector2D& Location) const override final;
	virtual FVector2D ProxyToLocation(const FVector2D& Proxy) const override final;

	virtual FVector LocationToProxy3D(const FVector& Location) const override final;
	virtual FVector ProxyToLocation3D(const FVector& Proxy) const override final;

protected:
	UPROPERTY(Instanced, EditInstanceOnly, Category = "Config", NoClear)
	TArray<TObjectPtr<UHeartNodeLocationModifier>> Modifiers;
};