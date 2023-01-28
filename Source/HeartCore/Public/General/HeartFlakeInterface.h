// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartFlakeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class HEARTCORE_API UHeartFlakeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class HEARTCORE_API IHeartFlakeInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "HeartFlakeInterface")
	void PreWrite();

	UFUNCTION(BlueprintNativeEvent, Category = "HeartFlakeInterface")
	void PostRead();
};
