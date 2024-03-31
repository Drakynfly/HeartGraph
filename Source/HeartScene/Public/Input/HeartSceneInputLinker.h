// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartInputLinkerBase.h"
#include "HeartSceneInputLinker.generated.h"

/**
 *
 */
UCLASS()
class HEARTSCENE_API UHeartSceneInputLinker : public UHeartInputLinkerBase
{
	GENERATED_BODY()

public:
	virtual bool InputKey(const FInputKeyParams& Params, UObject* Target);
};