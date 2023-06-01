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