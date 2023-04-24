// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartGraphInterface.generated.h"

class UHeartGraph;

UINTERFACE(NotBlueprintable)
class UHeartGraphInterface : public UInterface
{
	GENERATED_BODY()
};

class HEART_API IHeartGraphInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	virtual UHeartGraph* GetHeartGraph() const PURE_VIRTUAL(IHeartGraphInterface::GetHeartGraph, return nullptr; )
};

UINTERFACE()
class UHeartGraphInterfaceBP : public UHeartGraphInterface
{
	GENERATED_BODY()
};

class IHeartGraphInterfaceBP : public IHeartGraphInterface
{
	GENERATED_BODY()

protected:
	// Defer to Blueprint implementation
	virtual UHeartGraph* GetHeartGraph() const override final
	{
		return Execute_GetHeartGraph_BP(_getUObject());
	}

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|Graph", meta = (DisplayName = "Get Heart Graph"))
	UHeartGraph* GetHeartGraph_BP() const;
};
