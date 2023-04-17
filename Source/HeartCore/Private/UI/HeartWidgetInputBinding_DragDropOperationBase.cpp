// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputBinding_DragDropOperationBase.h"
#include "UI/HeartWidgetInputLinker.h"
#include "UI/HeartWidgetInputTrigger.h"


bool UHeartWidgetInputBinding_DragDropOperationBase::Bind(UHeartWidgetInputLinker* Linker,
                                                           const TArray<FInstancedStruct>& InTriggers) const
{
	Heart::Input::FConditionalDragDropTrigger DragDropTrigger;

	DragDropTrigger.Condition.BindUObject(this, &ThisClass::PassCondition);
	DragDropTrigger.Callback.BindUObject(this, &ThisClass::BeginDDO);
	DragDropTrigger.Layer = Heart::Input::Event;

	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			auto&& Trips = Trigger.Get<FHeartWidgetInputTrigger>().CreateTrips();

			for (auto&& Trip : Trips)
			{
				Linker->BindToOnDragDetected(Trip, DragDropTrigger);
			}
		}
	}

	return true;
}

bool UHeartWidgetInputBinding_DragDropOperationBase::Unbind(UHeartWidgetInputLinker* Linker,
	const TArray<FInstancedStruct>& InTriggers) const
{
	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			auto&& Trips = Trigger.Get<FHeartWidgetInputTrigger>().CreateTrips();

			for (auto&& Trip : Trips)
			{
				Linker->UnbindToOnDragDetected(Trip);
			}
		}
	}

	return true;
}
