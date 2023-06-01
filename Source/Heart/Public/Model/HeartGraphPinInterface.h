// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartGuids.h"
#include "HeartGraphPinInterface.generated.h"

class UHeartGraphNode;

UINTERFACE(NotBlueprintable)
class HEART_API UHeartGraphPinInterface : public UInterface
{
	GENERATED_BODY()
};

class HEART_API IHeartGraphPinInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|Pin")
	virtual UHeartGraphNode* GetHeartGraphNode() const PURE_VIRTUAL(IHeartGraphPinInterface::GetGraphNode, return nullptr; )

	UFUNCTION(BlueprintCallable, Category = "Heart|Pin")
	virtual FHeartPinGuid GetPinGuid() const PURE_VIRTUAL(IHeartGraphPinInterface::GetPinGuid, return FHeartPinGuid(); )
};

UINTERFACE()
class HEART_API UHeartGraphPinInterfaceBP : public UHeartGraphPinInterface
{
	GENERATED_BODY()
};

class HEART_API IHeartGraphPinInterfaceBP : public IHeartGraphPinInterface
{
	GENERATED_BODY()

protected:
	// Defer to Blueprint implementations

	virtual UHeartGraphNode* GetHeartGraphNode() const override final
	{
		return Execute_GetNode_BP(_getUObject());
	}

	virtual FHeartPinGuid GetPinGuid() const override final
	{
		return Execute_GetPinGuid_BP(_getUObject());
	}

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|Pin")
	UHeartGraphNode* GetNode_BP();

	UFUNCTION(BlueprintNativeEvent, Category = "Heart|Pin")
	FHeartPinGuid GetPinGuid_BP() const;
};