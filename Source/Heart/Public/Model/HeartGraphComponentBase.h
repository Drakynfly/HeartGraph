// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGuids.h"
#include "HeartGraphComponentBase.generated.h"

/**
 * Base class for component objects that can be added to Graphs or Nodes.
 */
UCLASS(Abstract)
class HEART_API UHeartGraphComponentBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;

	virtual void PostComponentAdded() {}
	virtual void PreComponentRemoved() {}

	FHeartExtensionGuid GetGuid() const { return Guid; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Extension")
	FHeartExtensionGuid Guid;
};