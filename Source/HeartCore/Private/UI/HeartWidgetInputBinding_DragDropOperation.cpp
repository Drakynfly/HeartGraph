// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBinding_DragDropOperation.h"

#include "UI/HeartWidgetInputLinker.h"
#include "UI/HeartWidgetInputTrigger.h"

bool UHeartWidgetInputBinding_DragDropOperation::Bind(UHeartWidgetInputLinker* Linker)
{
	for (auto&& Trigger : Triggers)
	{
		if (Trigger.IsValid())
		{
			UHeartWidgetInputLinker::FConditionalDragDropTrigger DragDropTrigger;
			DragDropTrigger.Class = OperationClass;
			DragDropTrigger.VisualClass = VisualClass;
			DragDropTrigger.Pivot = Pivot;
			DragDropTrigger.Offset = Offset;

			if (Condition)
			{
				DragDropTrigger.Condition = Condition->CreateCondition();
			}

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