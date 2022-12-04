// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Engine/Blueprint.h"
#include "HeartGraphNodeBlueprint.generated.h"

/**
 * A specialized blueprint class required for customizing Asset Type Actions
 */
UCLASS(BlueprintType)
class HEART_API UHeartGraphNodeBlueprint : public UBlueprint
{
	GENERATED_BODY()

public:
	UHeartGraphNodeBlueprint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	//~ UBlueprint
	virtual bool SupportedByDefaultBlueprintFactory() const override { return false; }
	virtual bool SupportsDelegates() const override { return false; }
	//~ UBlueprint
#endif
};
