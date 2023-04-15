// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartCanvasDragDropOperation.h"
#include "UI/HeartWidgetInputBinding_DragDropOperationBase.h"
#include "HeartCanvasActionDragDropOperation.generated.h"

class UHeartGraphCanvasAction;

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
	TObjectPtr<UHeartGraphCanvasAction> Action;
};

/**
 *
 */
UCLASS(meta = (DisplayName = "Drag Drop Action"))
class HEARTCANVAS_API UHeartWidgetInputBinding_DragDropOperation_Action : public UHeartWidgetInputBinding_DragDropOperationBase
{
	GENERATED_BODY()

protected:
	virtual UHeartDragDropOperation* BeginDDO(UWidget* Widget) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartGraphCanvasAction> ActionClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UUserWidget> VisualClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDragPivot Pivot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D Offset;
};