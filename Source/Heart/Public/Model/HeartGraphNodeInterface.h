// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.h"
#include "UObject/Interface.h"
#include "HeartGraphNodeInterface.generated.h"

class UHeartGraph;

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
	virtual UHeartGraph* GetHeartGraph() const PURE_VIRTUAL(IHeartGraphNodeInterface::GetHeartGraph, return nullptr; )

	UFUNCTION(BlueprintCallable, Category = "Heart|Node")
	virtual FHeartNodeGuid GetNodeGuid() const PURE_VIRTUAL(IHeartGraphNodeInterface::GetNodeGuid, return FHeartNodeGuid(); )
};