// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputHandlerBase.h"
#include "Input/HeartInputLinkerBase.h"
#include "Input/HeartInputTrigger.h"
#include "Input/HeartInputTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputHandlerBase)

bool UHeartInputHandlerBase::Bind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const
{
	using namespace Heart::Input;

	const TSharedRef<FConditionalCallback> Callback = MakeShared<FConditionalCallback>(
		FHandlerDelegate::CreateUObject(this, &ThisClass::OnTriggered),
		FDescriptionDelegate::CreateUObject(this, &ThisClass::GetDescription),
		FConditionDelegate::CreateUObject(this, &ThisClass::PassCondition),
		HandleInput ? Event : Listener);

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

bool UHeartInputHandlerBase::Unbind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const
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