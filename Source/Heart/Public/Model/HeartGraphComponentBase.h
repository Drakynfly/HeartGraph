// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGuids.h"
#include "HeartGraphComponentBase.generated.h"

class UHeartGraph;

/**
 * Base class for component objects that can be added to Graphs or Nodes.
 */
UCLASS(Abstract)
class HEART_API UHeartGraphComponentBase : public UObject
{
	GENERATED_BODY()

	friend UHeartGraph;

public:
	virtual void PostInitProperties() override;

	virtual void PostComponentAdded() {}
	virtual void PreComponentRemoved() {}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNodeComponent")
	UHeartGraph* GetGraph() const;

	FHeartExtensionGuid GetGuid() const { return Guid; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Extension")
	FHeartExtensionGuid Guid;
};