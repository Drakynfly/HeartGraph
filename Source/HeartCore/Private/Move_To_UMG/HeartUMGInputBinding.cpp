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

	FConditionalCallback Callback;

	auto&& DescDelegate = MakeShared<TLinkerType<UWidget>::FDescriptionDelegate>();
	DescDelegate->Delegate.BindDelegate(this, &ThisClass::GetDescription);
	Callback.Description = DescDelegate;

	auto&& ConditionDelegate = MakeShared<TLinkerType<UWidget>::FConditionDelegate>();
	ConditionDelegate->Delegate.BindDelegate(this, &ThisClass::PassCondition);
	Callback.Condition = ConditionDelegate;

	auto&& HandlerDeledage = MakeShared<TLinkerType<UWidget>::FHandlerDelegate>();
	HandlerDeledage->Delegate.BindDelegate(this, &ThisClass::TriggerEvent);
	Callback.Handler = HandlerDeledage;

	Callback.Layer = HandleInput ? Event : Listener;

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

	FConditionalCallback_DDO Callback;

	auto&& ConditionDelegate = MakeShared<TLinkerType<UWidget>::FConditionDelegate>();
	ConditionDelegate->Delegate.BindDelegate(this, &ThisClass::PassCondition);
	Callback.Condition = ConditionDelegate;

	auto&& DDOCreateDelegate = MakeShared<TLinkerType<UWidget>::FCreateDDODelegate>();
	DDOCreateDelegate->Delegate.BindDelegate(this, &ThisClass::BeginDDO);
	Callback.Handler = DDOCreateDelegate;

	Callback.Layer = Event;

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