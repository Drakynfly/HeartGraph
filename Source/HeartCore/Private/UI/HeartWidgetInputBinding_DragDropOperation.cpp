// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBinding_DragDropOperation.h"

#include "UI/HeartWidgetInputCondition.h"
#include "UI/HeartWidgetInputLinker.h"
#include "UI/HeartWidgetInputTrigger.h"

#include "Blueprint/UserWidget.h"

bool UHeartWidgetInputBinding_DragDropOperation::Bind(UHeartWidgetInputLinker* Linker)
{
	Heart::Input::FConditionalDragDropTrigger DragDropTrigger;

	DragDropTrigger.Callback =
		FHeartWidgetLinkedDragDropTriggerCreate::CreateWeakLambda(this, [this](UWidget* Widget)
			{
				auto&& NewDDO = NewObject<UHeartDragDropOperation>(GetTransientPackage(), OperationClass);

				if (IsValid(VisualClass))
				{
					NewDDO->DefaultDragVisual = CreateWidget(Widget, VisualClass);
					NewDDO->Pivot = Pivot;
					NewDDO->Offset = Offset;
				}

				return NewDDO;
			});

	DragDropTrigger.Layer = Heart::Input::Event;

	if (Condition)
	{
		DragDropTrigger.Condition = Condition->CreateCondition();
	}

	for (auto&& Trigger : Triggers)
	{
		if (Trigger.IsValid())
		{
			Linker->BindToOnDragDetected(Trigger.Get<FHeartWidgetInputTrigger>().CreateTrip(), DragDropTrigger);
		}
	}

	return true;
}

bool UHeartWidgetInputBinding_DragDropOperation::Unbind(UHeartWidgetInputLinker* Linker)
{
	for (auto&& Trigger : Triggers)
	{
		if (Trigger.IsValid())
		{
			Linker->UnbindToOnDragDetected(Trigger.GetMutable<FHeartWidgetInputTrigger>().CreateTrip());
		}
	}

	return true;
}