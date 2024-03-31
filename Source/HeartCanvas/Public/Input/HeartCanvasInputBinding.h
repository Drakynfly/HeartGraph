// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartInputBindingBase.h"
#include "HeartCanvasInputBinding.generated.h"

class UHeartDragDropOperation;
class UWidget;

/**
 * Base class for UMG Drag Drop Operation handlers
 */
UCLASS(Abstract)
class HEARTCANVAS_API UHeartCanvasInputHandler_DragDropOperationBase : public UHeartInputBindingBase
{
	GENERATED_BODY()

public:
	virtual bool Bind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;
	virtual bool Unbind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;

protected:
	virtual UHeartDragDropOperation* BeginDDO(UWidget* Widget) const
		PURE_VIRTUAL(UHeartWidgetInputBinding_DragDropOperationBase::BeginDDO, return nullptr; )
};