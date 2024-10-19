// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Model/HeartGraphInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartNodeLocationProxy.generated.h"

class UHeartNodeLocationModifier;

/*
 * A pass-through class for redirecting an accessor through proxy function to alter the location.
 */
UCLASS()
class HEART_API UHeartNodeLocationProxy : public UObject, public IHeartGraphInterface3D
{
	GENERATED_BODY()

public:
	/* IHeartGraphInterface */
	virtual UHeartGraph* GetHeartGraph() const override;
	virtual FVector2D GetNodeLocation(const FHeartNodeGuid& Node) const override final;
	virtual void SetNodeLocation(const FHeartNodeGuid& Node, const FVector2D& Location, bool InProgressMove) override final;
	/* IHeartGraphInterface */

	/* IHeartGraphInterface3D */
	virtual FVector GetNodeLocation3D(const FHeartNodeGuid& Node) const override final;
	virtual void SetNodeLocation3D(const FHeartNodeGuid& Node, const FVector& Location, bool InProgressMove) override final;
	/* IHeartGraphInterface3D */

public:
	static UHeartNodeLocationProxy* Create(UObject* ObjectToProxy, const TSubclassOf<UHeartNodeLocationProxy>& LocationProxyClass);

protected:
	UPROPERTY()
	TScriptInterface<IHeartGraphInterface> ProxiedObject;

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