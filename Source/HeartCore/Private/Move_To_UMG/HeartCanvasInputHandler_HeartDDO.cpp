// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Move_To_UMG/HeartCanvasInputHandler_HeartDDO.h"
#include "Move_To_UMG/HeartDragDropOperation.h"
#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasInputHandler_HeartDDO)

bool UHeartCanvasInputHandler_HeartDDO::PassCondition(const UWidget* TestWidget) const
{
	bool Failed = !Super::PassCondition(TestWidget);

	Failed |= !GetDefault<UHeartDragDropOperation>(OperationClass)->CanRunOnWidget(TestWidget);

	return !Failed;
}

UHeartDragDropOperation* UHeartCanvasInputHandler_HeartDDO::BeginDDO(UWidget* Widget) const
{
	auto&& NewDDO = NewObject<UHeartDragDropOperation>(GetTransientPackage(), OperationClass);

	if (IsValid(VisualClass))
	{
		NewDDO->DefaultDragVisual = CreateWidget(Widget, VisualClass);
		NewDDO->Pivot = Pivot;
		NewDDO->Offset = Offset;
	}

	return NewDDO;
}