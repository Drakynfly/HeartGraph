// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "InstancedStruct.h"
#include "HeartWidgetInputBindingBase.generated.h"

class UHeartWidgetInputLinker;
class UHeartWidgetInputCondition;

/**
 *
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, CollapseCategories)
class HEARTCORE_API UHeartWidgetInputBindingBase : public UObject
{
	GENERATED_BODY()

public:
	virtual bool Bind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) { return false; }
	virtual bool Unbind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) { return false; }

	// Optionally set condition
	UPROPERTY(EditAnywhere, Instanced, meta = (DisplayPriority = 100))
	TObjectPtr<UHeartWidgetInputCondition> Condition;

protected:
	UPROPERTY(EditAnywhere, meta = (BaseStruct = "/Script/HeartCore.HeartWidgetInputTrigger", ExcludeBaseStruct))
	TArray<FInstancedStruct> Triggers;
};
