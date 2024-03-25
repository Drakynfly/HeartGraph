// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartInputBindingBase.h"
#include "HeartCanvasInputBinding.generated.h"

struct FHeartInputActivation;
class UWidget;

/**
 * Base class for any UMG input handler
 */
UCLASS(Abstract)
class UHeartCanvasInputBindingBase : public UHeartInputBindingBase
{
	GENERATED_BODY()

public:
	HEARTCORE_API virtual FText GetDescription(const UWidget* TestWidget) const;
	HEARTCORE_API virtual bool PassCondition(const UWidget* TestWidget) const;
};

/**
 * Base class for simple UMG handlers
 */
UCLASS(Abstract)
class HEARTCORE_API UHeartCanvasInputHandlerBase : public UHeartCanvasInputBindingBase
{
	GENERATED_BODY()

public:
	// Bind triggers to OnTriggered function. Children should override that, instead of these.
	virtual bool Bind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;
	virtual bool Unbind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;

protected:
	virtual FReply OnTriggered(UWidget* Widget, const FHeartInputActivation& Trip) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Trigger")
	bool HandleInput = true;
};

class UHeartDragDropOperation;

/**
 * Base class for UMG Drag Drop Operation handlers
 */
UCLASS(Abstract)
class HEARTCORE_API UHeartCanvasInputHandler_DragDropOperationBase : public UHeartCanvasInputBindingBase
{
	GENERATED_BODY()

public:
	virtual bool Bind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;
	virtual bool Unbind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const override final;

protected:
	virtual UHeartDragDropOperation* BeginDDO(UWidget* Widget) const
		PURE_VIRTUAL(UHeartWidgetInputBinding_DragDropOperationBase::BeginDDO, return nullptr; )
};