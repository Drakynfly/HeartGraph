﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputBindingBase.h"
#include "HeartWidgetInputBinding_DragDropOperationBase.h"
#include "HeartWidgetInputBinding_DragDropOperation.generated.h"

enum class EDragPivot : uint8;

/**
 * A heart widget input binding that can launch a Drag Drop Operation
 */
UCLASS(meta = (DisplayName = "Drag Drop Operation"))
class HEARTCORE_API UHeartWidgetInputBinding_DragDropOperation : public UHeartWidgetInputBinding_DragDropOperationBase
{
	GENERATED_BODY()

protected:
	virtual bool PassCondition(const UWidget* TestWidget) const override;
	virtual UHeartDragDropOperation* BeginDDO(UWidget* Widget) const override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UHeartDragDropOperation> OperationClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowAbstract = "false"))
	TSubclassOf<UUserWidget> VisualClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDragPivot Pivot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D Offset;
};