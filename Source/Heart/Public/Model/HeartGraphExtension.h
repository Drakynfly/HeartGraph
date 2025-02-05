// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphComponentBase.h"
#include "HeartGraphExtension.generated.h"

class UHeartGraph;

/**
 * Extensions are the HeartGraph equivalent to 'ActorComponents'. They are added to graphs by their schema, or manually
 * at runtime.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class HEART_API UHeartGraphExtension : public UHeartGraphComponentBase
{
	GENERATED_BODY()

	friend UHeartGraph;

public:
	// Get the owning Heart Graph
	UFUNCTION(BlueprintCallable, Category = "Heart|Extension")
	UHeartGraph* GetGraph() const;

	virtual void PostComponentAdded() override
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		PostExtensionAdded();
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	}

	virtual void PreComponentRemoved() override
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		PreExtensionRemove();
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
	}

protected:
	UE_DEPRECATED(5.5, "Renamed to PostComponentAdded")
	virtual void PostExtensionAdded() {}

	UE_DEPRECATED(5.5, "Renamed to PreComponentRemove")
	virtual void PreExtensionRemove() {}
};