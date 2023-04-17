// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasActionDragDropOperation.h"
#include "Actions/HeartGraphCanvasAction.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "UI/HeartUMGContextObject.h"

bool UHeartCanvasActionDragDropOperation::SetupDragDropOperation()
{
	return IsValid(Action);
}

void UHeartCanvasActionDragDropOperation::Drop_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Drop_Implementation(PointerEvent);

	if (IsValid(Action))
	{
		FHeartInputActivation Activation;
		Activation.ActivationValue = 0;
		Action->Execute(GetHoveredWidget(), Activation, Payload);
	}
}

UHeartDragDropOperation* UHeartWidgetInputBinding_DragDropOperation_Action::BeginDDO(UWidget* Widget) const
{
	auto&& NewDDO = NewObject<UHeartCanvasActionDragDropOperation>(GetTransientPackage());

	if (IsValid(VisualClass))
	{
		auto&& NewVisual = CreateWidget(Widget, VisualClass);

		// If both the widget and the visual want a context object pass it between them
		if (Widget->Implements<UHeartUMGContextObject>() &&
			NewVisual->Implements<UHeartUMGContextObject>())
		{
			auto&& Context = IHeartUMGContextObject::Execute_GetContextObject(Widget);
			IHeartUMGContextObject::Execute_SetContextObject(NewVisual, Context);
		}

		NewDDO->DefaultDragVisual = NewVisual;
		NewDDO->Pivot = Pivot;
		NewDDO->Offset = Offset;
	}

	NewDDO->Action = NewObject<UHeartGraphCanvasAction>(NewDDO, ActionClass);

	return NewDDO;
}
