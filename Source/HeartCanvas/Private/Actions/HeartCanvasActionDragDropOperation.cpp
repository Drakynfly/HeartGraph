// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasActionDragDropOperation.h"
#include "Actions/HeartGraphCanvasAction.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "UI/HeartUMGContextObject.h"
#include "UI/HeartWidgetInputLinker.h"
#include "UI/HeartWidgetInputTrigger.h"

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

bool UHeartWidgetInputBinding_DragDropOperation_Action::Bind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const
{
	Heart::Input::FConditionalDragDropTrigger DragDropTrigger;

	if (Condition)
	{
		DragDropTrigger.Condition = Condition->CreateCondition();
	}

	DragDropTrigger.Callback = FHeartWidgetLinkedDragDropTriggerCreate::CreateUObject(this, &ThisClass::BeginDDO);
	DragDropTrigger.Layer = Heart::Input::Event;

	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			Linker->BindToOnDragDetected(Trigger.Get<FHeartWidgetInputTrigger>().CreateTrip(), DragDropTrigger);
		}
	}

	return true;
}

bool UHeartWidgetInputBinding_DragDropOperation_Action::Unbind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers) const
{
	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			Linker->UnbindToOnDragDetected(Trigger.GetMutable<FHeartWidgetInputTrigger>().CreateTrip());
		}
	}

	return true;
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
