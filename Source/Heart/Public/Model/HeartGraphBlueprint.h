// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Engine/Blueprint.h"
#include "HeartGraphBlueprint.generated.h"

/**
 *
 */
UCLASS()
class HEART_API UHeartGraphBlueprint : public UBlueprint
{
	GENERATED_BODY()

public:
	UHeartGraphBlueprint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	//~ UBlueprint
	virtual bool SupportedByDefaultBlueprintFactory() const override { return false; }
	//~ UBlueprint
#endif
};
