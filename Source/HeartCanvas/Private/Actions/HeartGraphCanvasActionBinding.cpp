// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartGraphCanvasActionBinding.h"
#include "Actions/HeartGraphCanvasAction.h"
#include "UMG/HeartGraphWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphCanvasActionBinding)

FText UHeartGraphCanvasActionBinding::GetDescription(const UWidget* TestWidget) const
{
	if (IsValid(ActionClass))
	{
		return ActionClass.GetDefaultObject()->GetDescription(TestWidget);
	}

	return Super::GetDescription(TestWidget);
}

bool UHeartGraphCanvasActionBinding::PassCondition(const UWidget* TestWidget) const
{
	bool Failed = !Super::PassCondition(TestWidget);

	if (IsValid(ActionClass))
	{
		if (auto&& HeartWidget = Cast<UHeartGraphWidgetBase>(TestWidget))
		{
			Failed |= !ActionClass.GetDefaultObject()->CanExecuteOnWidget(HeartWidget);
		}
	}

	return !Failed;
}

FReply UHeartGraphCanvasActionBinding::TriggerEvent(UWidget* Widget, const FHeartInputActivation& Trip) const
{
	if (auto&& HeartWidget = Cast<UHeartGraphWidgetBase>(Widget))
	{
		auto&& Action = UHeartGraphActionBase::CreateGraphAction<UHeartGraphCanvasAction>(ActionClass);

		if (!IsValid(Action))
		{
			return FReply::Unhandled();
		}

		return Action->ExecuteOnWidget(HeartWidget, Trip, nullptr);
	}

	return FReply::Unhandled();
}