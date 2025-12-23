// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartGuids.h"
#include "HeartGraphPinInterface.generated.h"

class UHeartGraph;

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
	virtual UHeartGraph* GetHeartGraph() const PURE_VIRTUAL(IHeartGraphPinInterface::GetHeartGraph, return nullptr; )

	UFUNCTION(BlueprintCallable, Category = "Heart|Pin")
	virtual FHeartNodeGuid GetNodeGuid() const PURE_VIRTUAL(IHeartGraphPinInterface::GetNodeGuid, return FHeartNodeGuid(); )

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

	virtual UHeartGraph* GetHeartGraph() const override final
	{
		return Execute_GetGraph_BP(Cast<UObject>(this));
	}

	virtual FHeartNodeGuid GetNodeGuid() const override final
	{
		return Execute_GetNodeGuid_BP(Cast<UObject>(this));
	}

	virtual FHeartPinGuid GetPinGuid() const override final
	{
		return Execute_GetPinGuid_BP(Cast<UObject>(this));
	}

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|Pin")
	UHeartGraph* GetGraph_BP() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Heart|Pin")
	FHeartNodeGuid GetNodeGuid_BP() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Heart|Pin")
	FHeartPinGuid GetPinGuid_BP() const;
};