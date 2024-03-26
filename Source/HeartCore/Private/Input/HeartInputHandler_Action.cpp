// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputHandler_Action.h"
#include "Input/HeartActionBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputHandler_Action)

FText UHeartInputHandler_Action::GetDescription(const UObject* TestTarget) const
{
	if (IsValid(ActionClass))
	{
		return ActionClass.GetDefaultObject()->GetDescription(TestTarget);
	}

	return Super::GetDescription(TestTarget);
}

bool UHeartInputHandler_Action::PassCondition(const UObject* TestTarget) const
{
	bool Failed = !Super::PassCondition(TestTarget);

	if (IsValid(ActionClass))
	{
		Failed |= !ActionClass.GetDefaultObject()->CanExecute(TestTarget);
	}

	return !Failed;
}

FReply UHeartInputHandler_Action::OnTriggered(UObject* Target, const FHeartInputActivation& Activation) const
{
	if (IsValid(Target))
	{
		auto&& Action = UHeartActionBase::CreateGraphAction(ActionClass);

		Heart::Action::FArguments Args;
		Args.Target = Target;
		Args.Activation = Activation;

		if (Action->Execute(Args))
		{
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}