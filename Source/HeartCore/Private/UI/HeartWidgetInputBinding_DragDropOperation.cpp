// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBinding_DragDropOperation.h"
#include "UI/HeartWidgetInputCondition.h"
#include "UI/HeartWidgetInputLinker.h"
#include "UI/HeartWidgetInputTrigger.h"

bool UHeartWidgetInputBinding_DragDropOperation::Bind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers)
{
	Heart::Input::FConditionalDragDropTrigger DragDropTrigger;
	DragDropTrigger.Class = OperationClass;
	DragDropTrigger.VisualClass = VisualClass;
	DragDropTrigger.Pivot = Pivot;
	DragDropTrigger.Offset = Offset;
	DragDropTrigger.Layer = Heart::Input::Event;

	if (Condition)
	{
		DragDropTrigger.Condition = Condition->CreateCondition();
	}

	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			Linker->BindToOnDragDetected(Trigger.Get<FHeartWidgetInputTrigger>().CreateTrip(), DragDropTrigger);
		}
	}

	return true;
}

bool UHeartWidgetInputBinding_DragDropOperation::Unbind(UHeartWidgetInputLinker* Linker, const TArray<FInstancedStruct>& InTriggers)
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