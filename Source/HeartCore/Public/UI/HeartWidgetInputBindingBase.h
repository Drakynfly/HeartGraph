// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "InstancedStruct.h"
#include "HeartWidgetInputBindingBase.generated.h"

class UWidget;
class UHeartWidgetInputLinker;
class UHeartWidgetInputCondition;

/**
 *
 */
UCLASS(Abstract, const, DefaultToInstanced, EditInlineNew, CollapseCategories)
class HEARTCORE_API UHeartWidgetInputBindingBase : public UObject
{
	GENERATED_BODY()

public:
	virtual FText GetDescription(const UWidget* TestWidget) const;
	virtual bool PassCondition(const UWidget* TestWidget) const;

	virtual bool Bind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const { return false; }
	virtual bool Unbind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const { return false; }

protected:
	// Optionally set condition
	UPROPERTY(EditAnywhere, Instanced, meta = (DisplayPriority = 100), meta = (NoResetToDefault))
	TObjectPtr<UHeartWidgetInputCondition> Condition;
};
