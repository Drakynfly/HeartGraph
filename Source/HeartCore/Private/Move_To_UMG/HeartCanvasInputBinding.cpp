// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Move_To_UMG/HeartCanvasInputBinding.h"
#include "Input/HeartInputLinkerBase.h"
#include "Input/HeartInputTrigger.h"
#include "Input/HeartInputTypes.h"
#include "Move_To_UMG/HeartUMGInputLinker.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasInputBinding)

bool UHeartCanvasInputHandler_DragDropOperationBase::Bind(UHeartInputLinkerBase* Linker,
														   const TArray<FInstancedStruct>& InTriggers) const
{
	using namespace Heart::Input;

	auto UMGLinker = Cast<UHeartWidgetInputLinker>(Linker);
	if (!IsValidChecked(UMGLinker))
	{
		return false;
	}

	const TSharedRef<FConditionalCallback_DDO> Callback = MakeShared<FConditionalCallback_DDO>(
		MakeShared<TLinkerType<UWidget>::FCreateDDODelegate>(this, &ThisClass::BeginDDO),
		FDescriptionDelegate::CreateUObject(this, &ThisClass::GetDescription),
		FConditionDelegate::CreateUObject(this, &ThisClass::PassCondition),
		Event );

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

bool UHeartCanvasInputHandler_DragDropOperationBase::Unbind(UHeartInputLinkerBase* Linker,
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