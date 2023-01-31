// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartWidgetInputBindingBase.h"
#include "HeartDragDropOperation.h"
#include "HeartWidgetInputBinding_DragDropOperation.generated.h"

/**
 *
 */
UCLASS(meta = (DisplayName = "Drag Drop Operation"))
class HEARTCORE_API UHeartWidgetInputBinding_DragDropOperation : public UHeartWidgetInputBindingBase
{
	GENERATED_BODY()

public:
	virtual bool Bind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const override;
	virtual bool Unbind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const override;

protected:
	UHeartDragDropOperation* BeginDDO(UWidget* Widget) const;

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