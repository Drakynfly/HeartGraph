// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "InstancedStruct.h"
#include "HeartInputHandlerAssetBase.generated.h"

class UHeartInputLinkerBase;

/**
 * Base class for all Input Handler assets.
 */
UCLASS(Abstract, const)
class HEARTCORE_API UHeartInputHandlerAssetBase : public UObject
{
	GENERATED_BODY()

public:
	virtual bool Bind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const { return false; }
	virtual bool Unbind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const { return false; }
};