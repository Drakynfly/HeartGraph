// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Move_To_UMG/HeartUMGInputBinding_DragDropOperation.h"
#include "Move_To_UMG/HeartDragDropOperation.h"
#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartUMGInputBinding_DragDropOperation)

bool UHeartUMGInputBinding_DragDropOperation::PassCondition(const UWidget* TestWidget) const
{
	bool Failed = !Super::PassCondition(TestWidget);

	Failed |= !GetDefault<UHeartDragDropOperation>(OperationClass)->CanRunOnWidget(TestWidget);

	return !Failed;
}

UHeartDragDropOperation* UHeartUMGInputBinding_DragDropOperation::BeginDDO(UWidget* Widget) const
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