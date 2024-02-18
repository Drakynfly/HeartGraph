// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Model/HeartGraphInterface.h"
#include "UObject/Object.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartNodeLocationAccessor.generated.h"

struct FHeartNodeGuid;
class UHeartGraph;
class UHeartGraphNode;
class UHeartGraphNode3D;

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class HEART_API UHeartNodeLocationAccessor : public UHeartGraphInterface
{
	GENERATED_BODY()
};

/**
 * This must be added to classes that display node positions visually, in order for other systems (like layout
 * automation) to interop with them.
 */
class HEART_API IHeartNodeLocationAccessor : public IHeartGraphInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location (Guid)"))
	virtual FVector2D GetNodeLocation(const FHeartNodeGuid& Node) const;

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location (Guid)"))
	virtual void SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, bool InProgressMove);

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location 3D (Guid)"))
	virtual FVector GetNodeLocation3D(const FHeartNodeGuid& Node) const;

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location 3D (Guid)"))
	virtual void SetNodeLocation3D(const FHeartNodeGuid& Node, const FVector& Location, bool InProgressMove);
};


UCLASS()
class HEART_API UHeartNodeLocationAccessorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location"))
	static FVector2D GetNodeLocation_Pointer(const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, UHeartGraphNode* Node);

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location"))
	static void SetNodeLocation_Pointer(const TScriptInterface<IHeartNodeLocationAccessor>& Accessor,
		UHeartGraphNode* Node, const FVector2D& Location, bool InProgressMove);

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location 3D"))
	static FVector GetNodeLocation3D_Pointer(const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, UHeartGraphNode3D* Node);

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location 3D"))
	static void SetNodeLocation3D_Pointer(const TScriptInterface<IHeartNodeLocationAccessor>& Accessor,
		UHeartGraphNode3D* Node, const FVector& Location, bool InProgressMove);
};


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

/*
 * A pass-through class for redirecting an accessor through proxy function to alter the location.
 */
UCLASS()
class HEART_API UHeartNodeLocationProxy : public UObject, public IHeartNodeLocationAccessor
{
	GENERATED_BODY()

public:
	/* IHeartNodeLocationAccessor */
	virtual UHeartGraph* GetHeartGraph() const override;
	virtual FVector2D GetNodeLocation(const FHeartNodeGuid& Node) const override final;
	virtual void SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, bool InProgressMove) override final;
	virtual FVector GetNodeLocation3D(const FHeartNodeGuid& Node) const override final;
	virtual void SetNodeLocation3D(const FHeartNodeGuid& Node, const FVector& Location, bool InProgressMove) override final;
	/* IHeartNodeLocationAccessor */

public:
	static UHeartNodeLocationProxy* Create(UObject* ObjectToProxy, TSubclassOf<UHeartNodeLocationProxy> LocationProxyClass);

protected:
	UPROPERTY()
	TScriptInterface<IHeartNodeLocationAccessor> ProxiedObject;

	UPROPERTY()
	TObjectPtr<UHeartNodeLocationModifier> ProxyLayer;
};

UCLASS()
class HEART_API UHeartNodeLocationProxyLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|NodeLocationProxyLibrary")
	static UHeartNodeLocationProxy* CreateNodeLocationProxy(UObject* ObjectToProxy, TSubclassOf<UHeartNodeLocationProxy> LocationProxyClass);
};