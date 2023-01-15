// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartUMGContextObject.generated.h"

// This class does not need to be modified.
UINTERFACE()
class HEARTCORE_API UHeartUMGContextObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * A simple interface to communicate with widgets that use a context object.
 */
class HEARTCORE_API IHeartUMGContextObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Context Object")
	void SetContextObject(UObject* Object);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Context Object")
	UObject* GetContextObject() const;
};
