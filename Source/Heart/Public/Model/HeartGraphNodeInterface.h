// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartGraphNodeInterface.generated.h"

class UHeartGraphNode;

UINTERFACE(NotBlueprintable)
class HEART_API UHeartGraphNodeInterface : public UInterface
{
	GENERATED_BODY()
};

class HEART_API IHeartGraphNodeInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	virtual UHeartGraphNode* GetHeartGraphNode() const PURE_VIRTUAL(IHeartGraphNodeInterface::GetHeartGraphNode, return nullptr; )
};


UINTERFACE()
class UHeartGraphNodeInterfaceBP : public UHeartGraphNodeInterface
{
	GENERATED_BODY()
};

class IHeartGraphNodeInterfaceBP : public IHeartGraphNodeInterface
{
	GENERATED_BODY()

protected:
	// Defer to Blueprint implementation
	virtual UHeartGraphNode* GetHeartGraphNode() const override final
	{
		return Execute_GetHeartGraphNode_BP(_getUObject());
	}

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|Graph", meta = (DisplayName = "Get Heart Graph Node"))
	UHeartGraphNode* GetHeartGraphNode_BP() const;
};