// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputHandler_Action.h"
#include "Input/HeartActionBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputHandler_Action)

FText UHeartInputHandler_Action::GetDescription(const UObject* TestTarget) const
{
	if (IsValid(ActionClass))
	{
		return Heart::Action::GetDescription(ActionClass, TestTarget);
	}

	return Super::GetDescription(TestTarget);
}

bool UHeartInputHandler_Action::PassCondition(const UObject* TestTarget) const
{
	bool Failed = !Super::PassCondition(TestTarget);

	if (IsValid(ActionClass))
	{
		Failed |= !Heart::Action::CanExecute(ActionClass, TestTarget);
	}

	return !Failed;
}

FHeartEvent UHeartInputHandler_Action::OnTriggered(UObject* Target, const FHeartInputActivation& Activation) const
{
	if (!ensure(IsValid(Target)))
	{
		// Target is null, how did we get here?
		return FHeartEvent::Invalid;
	}

	return Heart::Action::Execute(ActionClass, Target, Activation);
}