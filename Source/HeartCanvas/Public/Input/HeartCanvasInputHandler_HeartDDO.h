// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartCanvasInputBinding.h"
#include "HeartCanvasInputHandler_HeartDDO.generated.h"

enum class EDragPivot : uint8;

/**
 * A heart input handler that can launch a Drag Drop Operation
 */
UCLASS(meta = (DisplayName = "Drag Drop Operation"))
class HEARTCANVAS_API UHeartCanvasInputHandler_HeartDDO : public UHeartCanvasInputHandler_DragDropOperationBase
{
	GENERATED_BODY()

protected:
	virtual bool PassCondition(const UObject* TestTarget) const override;
	virtual UHeartDragDropOperation* BeginDDO(UWidget* Widget) const override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Canvas DDO", meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartDragDropOperation> OperationClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual", meta = (AllowAbstract = "false"))
	TSubclassOf<UUserWidget> VisualClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual", meta = (EditCondition = "VisualClass != nullptr", HideEditConditionToggle))
	EDragPivot Pivot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual", meta = (EditCondition = "VisualClass != nullptr", HideEditConditionToggle))
	FVector2D Offset;
};