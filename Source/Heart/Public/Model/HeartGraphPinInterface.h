// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartGuids.h"
#include "HeartGraphPinInterface.generated.h"

class UHeartGraphNode;

UINTERFACE()
class HEART_API UHeartGraphPinInterface : public UInterface
{
	GENERATED_BODY()
};

class HEART_API IHeartGraphPinInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Heart|Pin")
	UHeartGraphNode* GetNode();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Heart|Pin")
	FHeartPinGuid GetPinGuid() const;
};
