// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartCanvasInputHandler_Action.h"
#include "Actions/HeartGraphCanvasAction.h"
#include "UMG/HeartGraphWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasInputHandler_Action)

FText UHeartCanvasInputHandler_Action::GetDescription(const UWidget* TestWidget) const
{
	if (IsValid(ActionClass))
	{
		return ActionClass.GetDefaultObject()->GetDescription(TestWidget);
	}

	return Super::GetDescription(TestWidget);
}

bool UHeartCanvasInputHandler_Action::PassCondition(const UWidget* TestWidget) const
{
	bool Failed = !Super::PassCondition(TestWidget);

	if (IsValid(ActionClass))
	{
		Failed |= !ActionClass.GetDefaultObject()->CanExecute(TestWidget);
	}

	return !Failed;
}

FReply UHeartCanvasInputHandler_Action::OnTriggered(UWidget* Widget, const FHeartInputActivation& Activation) const
{
	if (IsValid(Widget))
	{
		auto&& Action = UHeartGraphActionBase::CreateGraphAction(ActionClass);

		Heart::Action::FArguments Args;
		Args.Target = Widget;
		Args.Activation = Activation;

		if (Action->Execute(Args))
		{
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}