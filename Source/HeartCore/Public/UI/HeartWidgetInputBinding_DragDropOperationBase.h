// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputBindingBase.h"
#include "HeartWidgetInputBinding_DragDropOperationBase.generated.h"

class UHeartDragDropOperation;

/**
 *
 */
UCLASS()
class HEARTCORE_API UHeartWidgetInputBinding_DragDropOperationBase : public UHeartWidgetInputBindingBase
{
	GENERATED_BODY()

public:
	virtual bool Bind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;
	virtual bool Unbind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;

protected:
	virtual UHeartDragDropOperation* BeginDDO(UWidget* Widget) const
		PURE_VIRTUAL(UHeartWidgetInputBinding_DragDropOperationBase::BeginDDO, return nullptr; )
};
