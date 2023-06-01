// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "InstancedStruct.h"
#include "HeartWidgetInputHandlerAsset.generated.h"

class UWidget;
class UHeartWidgetInputLinker;

/**
 *
 */
UCLASS(Abstract, const)
class HEARTCORE_API UHeartWidgetInputHandlerAsset : public UObject
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(const UWidget* TestWidget) const;
	virtual bool PassCondition(const UWidget* TestWidget) const;

	virtual bool Bind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const { return false; }
	virtual bool Unbind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const { return false; }
};
