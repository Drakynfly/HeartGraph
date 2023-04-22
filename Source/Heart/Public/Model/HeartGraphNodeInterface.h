// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartGraphNodeInterface.generated.h"

class UHeartGraphNode;

UINTERFACE()
class HEART_API UHeartGraphNodeInterface : public UInterface
{
	GENERATED_BODY()
};

class HEART_API IHeartGraphNodeInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Heart|Node")
	UHeartGraphNode* GetHeartGraphNode() const;
};
