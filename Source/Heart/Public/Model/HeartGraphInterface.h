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

class HEART_API IHeartGraphInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Heart|Graph")
	UHeartGraph* GetHeartGraph() const;

	UHeartGraph* GetHeartGraphNative() const
	{
		return Execute_GetHeartGraph(_getUObject());
	}
};
