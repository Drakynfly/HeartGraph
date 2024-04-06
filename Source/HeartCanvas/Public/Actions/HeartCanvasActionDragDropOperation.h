// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartCanvasDragDropOperation.h"
#include "Input/HeartCanvasInputBinding.h"
#include "HeartCanvasActionDragDropOperation.generated.h"

class UHeartActionBase;

/**
 * A drag drop operation that carries a UHeartGraphCanvasAction, which will attempt to run on the widget dropped on.
 */
UCLASS(HideDropdown)
class HEARTCANVAS_API UHeartCanvasActionDragDropOperation : public UHeartCanvasDragDropOperation
{
	GENERATED_BODY()

public:
	virtual bool SetupDragDropOperation() override;
	virtual void Drop_Implementation(const FPointerEvent& PointerEvent) override;

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn))
	TSubclassOf<UHeartActionBase> Action;
};

/**
 *
 */
UCLASS(meta = (DisplayName = "Drag Drop Action"))
class HEARTCANVAS_API UHeartCanvasInputHandler_DDO_Action : public UHeartCanvasInputHandler_DragDropOperationBase
{
	GENERATED_BODY()

protected:
	virtual bool PassCondition(const UObject* TestTarget) const override;
	virtual UHeartDragDropOperation* BeginDDO(UWidget* Widget) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Canvas DDO", meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartActionBase> ActionClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual", meta = (AllowAbstract = "false"))
	TSubclassOf<UUserWidget> VisualClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual", meta = (EditCondition = "VisualClass != nullptr", HideEditConditionToggle))
	EDragPivot Pivot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual", meta = (EditCondition = "VisualClass != nullptr", HideEditConditionToggle))
	FVector2D Offset;
};