// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphComponentBase.h"
#include "HeartGraphExtension.generated.h"

/**
 * Extensions are the HeartGraph equivalent to 'ActorComponents'. They are added to graphs by their schema, or manually
 * at runtime.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class HEART_API UHeartGraphExtension : public UHeartGraphComponentBase
{
	GENERATED_BODY()
};