// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartGraphInterface.generated.h"

class UHeartGraph;

UINTERFACE()
class UHeartGraphInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Allows any class to expose a Heart Graph it represents. Blueprint actors/components can implement this as well, in
 * order to utilize Heart APIs that consume this interface.
 */
class HEART_API IHeartGraphInterface
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Heart|Graph", meta = (DisplayName = "Get Heart Graph"))
	UHeartGraph* GetHeartGraph() const;
};