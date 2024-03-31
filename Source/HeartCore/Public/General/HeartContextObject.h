// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "HeartContextObject.generated.h"

// This class does not need to be modified.
UINTERFACE()
class HEARTCORE_API UHeartContextObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * A simple interface to communicate with any class that uses a context object.
 */
class HEARTCORE_API IHeartContextObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Context Object")
	void SetContextObject(UObject* Object);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Context Object")
	UObject* GetContextObject() const;
};