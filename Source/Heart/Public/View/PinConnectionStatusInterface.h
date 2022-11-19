// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "ModelView/HeartGraphSchema.h"
#include "PinConnectionStatusInterface.generated.h"

UINTERFACE()
class HEART_API UPinConnectionStatusInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * A generic interface to receive notifications about an attempt to connect pins from a schema.
 */
class HEART_API IPinConnectionStatusInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Heart|PinConnectionStatusInterface")
	void SetConnectionResponse(const FHeartConnectPinsResponse& Response);
};
