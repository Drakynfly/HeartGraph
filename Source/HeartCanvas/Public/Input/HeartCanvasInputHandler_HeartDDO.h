// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Input/HeartInputHandlerAssetBase.h"
#include "Templates/SubclassOf.h"
#include "HeartCanvasInputHandler_HeartDDO.generated.h"

class UUserWidget;
enum class EDragPivot : uint8;
class UHeartDragDropOperation;

/**
 * A heart input handler that can launch a Drag Drop Operation
 */
UCLASS(meta = (DisplayName = "Drag Drop Operation"))
class HEARTCANVAS_API UHeartCanvasInputHandler_HeartDDO : public UHeartInputHandlerAssetBase
{
	GENERATED_BODY()

protected:
	virtual Heart::Input::EExecutionOrder GetExecutionOrder() const override { return Heart::Input::Deferred; }
	virtual bool PassCondition(const UObject* TestTarget) const override;
	virtual FHeartEvent OnTriggered(UObject* Target, const FHeartInputActivation& Activation) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Canvas DDO", meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartDragDropOperation> OperationClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual", meta = (AllowAbstract = "false"))
	TSubclassOf<UUserWidget> VisualClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual", meta = (EditCondition = "VisualClass != nullptr", HideEditConditionToggle))
	EDragPivot Pivot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visual", meta = (EditCondition = "VisualClass != nullptr", HideEditConditionToggle))
	FVector2D Offset;
};