// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Move_To_UMG/HeartUMGInputBinding.h"
#include "Move_To_UMG/HeartUMGInputLinker.h"
#include "Input/HeartInputTrigger.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartUMGInputBinding)

FText UHeartUMGInputBindingBase::GetDescription(const UWidget* TestWidget) const
{
	return FText::GetEmpty();
}

bool UHeartUMGInputBindingBase::PassCondition(const UWidget* TestWidget) const
{
	return true;
}

bool UHeartUMGInputBinding_Handler::Bind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const
{
	using namespace Heart::Input;

	const FConditionalCallback Callback{
		MakeShared<TLinkerType<UWidget>::FDescriptionDelegate>(this, &ThisClass::GetDescription),
		MakeShared<TLinkerType<UWidget>::FConditionDelegate>(this, &ThisClass::PassCondition),
		HandleInput ? Event : Listener,
		MakeShared<TLinkerType<UWidget>::FHandlerDelegate>(this, &ThisClass::TriggerEvent)
	};

	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			auto&& Trips = Trigger.Get<FHeartInputTrigger>().CreateTrips();

			for (auto&& Trip : Trips)
			{
				Linker->BindInputCallback(Trip, Callback);
			}
		}
	}

	return true;
}

bool UHeartUMGInputBinding_Handler::Unbind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const
{
	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			auto&& Trips = Trigger.Get<FHeartInputTrigger>().CreateTrips();

			for (auto&& Trip : Trips)
			{
				Linker->UnbindInputCallback(Trip);
			}
		}
	}

	return true;
}

FReply UHeartUMGInputBinding_Handler::TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const
{
	return FReply::Unhandled();
}

bool UHeartUMGInputBinding_DragDropOperationBase::Bind(UHeartInputLinkerBase* Linker,
														   const TArray<FInstancedStruct>& InTriggers) const
{
	using namespace Heart::Input;

	auto UMGLinker = Cast<UHeartWidgetInputLinker>(Linker);
	if (!IsValidChecked(UMGLinker))
	{
		return false;
	}

	const FConditionalCallback_DDO Callback {
		MakeShared<TLinkerType<UWidget>::FDescriptionDelegate>(this, &ThisClass::GetDescription),
		MakeShared<TLinkerType<UWidget>::FConditionDelegate>(this, &ThisClass::PassCondition),
		Event,
		MakeShared<TLinkerType<UWidget>::FCreateDDODelegate>(this, &ThisClass::BeginDDO)
	};

	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			auto&& Trips = Trigger.Get<FHeartInputTrigger>().CreateTrips();

			for (auto&& Trip : Trips)
			{
				UMGLinker->BindToOnDragDetected(Trip, Callback);
			}
		}
	}

	return true;
}

bool UHeartUMGInputBinding_DragDropOperationBase::Unbind(UHeartInputLinkerBase* Linker,
	const TArray<FInstancedStruct>& InTriggers) const
{
	auto UMGLinker = Cast<UHeartWidgetInputLinker>(Linker);
	if (!IsValidChecked(UMGLinker))
	{
		return false;
	}

	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			auto&& Trips = Trigger.Get<FHeartInputTrigger>().CreateTrips();

			for (auto&& Trip : Trips)
			{
				UMGLinker->UnbindToOnDragDetected(Trip);
			}
		}
	}

	return true;
}