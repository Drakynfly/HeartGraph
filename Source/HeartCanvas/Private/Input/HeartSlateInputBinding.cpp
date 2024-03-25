// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartSlateInputBinding.h"
#include "Input/HeartInputTrigger.h"
#include "Input/HeartSlateInputLinker.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartSlateInputBinding)

FText UHeartSlateInputBindingBase::GetDescription(const TSharedRef<SWidget>& TestWidget) const
{
	return FText::GetEmpty();
}

bool UHeartSlateInputBindingBase::PassCondition(const TSharedRef<SWidget>& TestWidget) const
{
	return true;
}

bool UHeartSlateInputHandlerBase::Bind(UHeartInputLinkerBase* Linker, const TArray<FInstancedStruct>& InTriggers) const
{
	using namespace Heart::Input;

	auto SlateLinker = Cast<UHeartSlateInputLinker>(Linker);
	if (!IsValidChecked(SlateLinker))
	{
		return false;
	}

	const FConditionalCallback Callback{
		MakeShared<TLinkerType<SWidget>::FDescriptionDelegate>(this, &ThisClass::GetDescription),
		MakeShared<TLinkerType<SWidget>::FConditionDelegate>(this, &ThisClass::PassCondition),
		HandleInput ? Event : Listener,
		MakeShared<TLinkerType<SWidget>::FHandlerDelegate>(this, &ThisClass::OnTriggered)
	};

	for (auto&& Trigger : InTriggers)
	{
		if (Trigger.IsValid())
		{
			auto&& Trips = Trigger.Get<FHeartInputTrigger>().CreateTrips();

			for (auto&& Trip : Trips)
			{
				SlateLinker->BindInputCallback(Trip, Callback);
			}
		}
	}

	return true;
}

bool UHeartSlateInputHandlerBase::Unbind(UHeartInputLinkerBase* Linker,
	const TArray<FInstancedStruct>& InTriggers) const
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

FReply UHeartSlateInputHandlerBase::OnTriggered(TSharedRef<SWidget>& Widget,
	const FHeartInputActivation& Trip) const
{
	return FReply::Unhandled();
}