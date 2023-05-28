// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBinding_DragDropOperation.h"
#include "Blueprint/UserWidget.h"
#include "UI/HeartDragDropOperation.h"

bool UHeartWidgetInputBinding_DragDropOperation::PassCondition(const UWidget* TestWidget) const
{
	bool Failed = !Super::PassCondition(TestWidget);

	Failed |= !GetDefault<UHeartDragDropOperation>(OperationClass)->CanRunOnWidget(TestWidget);

	return !Failed;
}

UHeartDragDropOperation* UHeartWidgetInputBinding_DragDropOperation::BeginDDO(UWidget* Widget) const
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
