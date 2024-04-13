// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartCanvasDragDropOperation.h"
#include "Input/HeartInputHandlerAssetBase.h"
#include "HeartCanvasInputHandler_DDO_Action.generated.h"

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

namespace Heart::Canvas
{
	/**
	 * A native drag drop operation that carries a UHeartGraphCanvasAction, which will attempt to run on the widget dropped on.
	 */
	class FSlateActionDragDropOperation : public FNativeDragDropOperation
	{
	public:
		DRAG_DROP_OPERATOR_TYPE(FHeartSlateActionDragDropOperation, FNativeDragDropOperation)

		virtual FReply OnHoverWidget(const TSharedRef<SWidget>& Widget) override;

		virtual bool SetupDragDropOperation() override;
		virtual void OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent) override;

		TSubclassOf<UHeartActionBase> Action;

	private:
		TSharedPtr<SWidget> LastHovered;
	};
}

/**
 *
 */
UCLASS(meta = (DisplayName = "Drag Drop Action"))
class HEARTCANVAS_API UHeartCanvasInputHandler_DDO_Action : public UHeartInputHandlerAssetBase
{
	GENERATED_BODY()

protected:
	virtual EHeartInputExecutionOrder GetExecutionOrder() const override { return EHeartInputExecutionOrder::Deferred; }
	virtual bool PassCondition(const UObject* TestTarget) const override;
	virtual FHeartEvent OnTriggered(UObject* Target, const FHeartInputActivation& Activation) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Canvas DDO", meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartActionBase> ActionClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual", meta = (AllowAbstract = "false"))
	TSubclassOf<UUserWidget> VisualClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual", meta = (EditCondition = "VisualClass != nullptr", HideEditConditionToggle))
	EDragPivot Pivot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual", meta = (EditCondition = "VisualClass != nullptr", HideEditConditionToggle))
	FVector2D Offset;
};