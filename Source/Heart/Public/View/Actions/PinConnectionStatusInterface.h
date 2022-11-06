// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "ModelView/HeartGraphSchema.h"
#include "UObject/Interface.h"
#include "PinConnectionStatusInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPinConnectionStatusInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class HEART_API IPinConnectionStatusInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetConnectionResponse(const FHeartConnectPinsResponse& Response);
};
