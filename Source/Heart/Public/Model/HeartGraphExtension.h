// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.h"
#include "UObject/Object.h"
#include "HeartGraphExtension.generated.h"

class UHeartGraph;

/**
 * Extensions are the HeartGraph equivalent to 'ActorComponents'. They are added to graphs by their schema, or manually
 * at runtime.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, Within = HeartGraph)
class HEART_API UHeartGraphExtension : public UObject
{
	GENERATED_BODY()

	friend UHeartGraph;

public:
	virtual void PostInitProperties() override;

	FHeartExtensionGuid GetGuid() const { return Guid; }

	// Get the owning Heart Graph
	UFUNCTION(BlueprintCallable, Category = "Heart|Extension")
	UHeartGraph* GetGraph() const;

protected:
	virtual void PostExtensionAdded() {}
	virtual void PreExtensionRemove() {}

	UPROPERTY(BlueprintReadOnly, Category = "Extension")
	FHeartExtensionGuid Guid;
};