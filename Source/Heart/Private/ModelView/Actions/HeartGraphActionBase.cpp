// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartGraphActionBase.h"

#include "UI/HeartInputActivation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphActionBase)

bool UHeartGraphActionBase::QuickExecuteGraphAction(const TSubclassOf<UHeartGraphActionBase> Class,
													UObject* Target, const FHeartManualEvent& Activation)
{
	if (!ensure(IsValid(Class)))
	{
		return false;
	}

	if (!Class->GetDefaultObject<UHeartGraphActionBase>()->CanExecute(Target))
	{
		return false;
	}

	auto&& Action = CreateGraphAction(Class);

	Heart::Action::FArguments Args;
	Args.Activation = Activation;

	return Action->Execute(Target, Args);
}

bool UHeartGraphActionBase::QuickExecuteGraphActionWithPayload(const TSubclassOf<UHeartGraphActionBase> Class,
                                                               UObject* Target, const FHeartManualEvent& Activation, UObject* Payload)
{
	if (!ensure(IsValid(Class)))
	{
		return false;
	}

	if (!Class->GetDefaultObject<UHeartGraphActionBase>()->CanExecute(Target))
	{
		return false;
	}

	auto&& Action = CreateGraphAction(Class);

	Heart::Action::FArguments Args;
	Args.Activation = Activation;
	Args.Payload = Payload;

	return Action->Execute(Target, Args);
}

UHeartGraphActionBase* UHeartGraphActionBase::CreateGraphAction(const TSubclassOf<UHeartGraphActionBase> Class)
{
	if (!ensure(IsValid(Class)))
	{
		return nullptr;
	}

	return NewObject<UHeartGraphActionBase>(GetTransientPackage(), Class);
}

bool UHeartGraphActionBase::ExecuteGraphAction(UHeartGraphActionBase* Action, UObject* Target, const FHeartManualEvent& Activation)
{
	if (ensure(Action))
	{
		if (!Action->CanExecute(Target))
		{
			return false;
		}

		Heart::Action::FArguments Args;
		Args.Activation = Activation;

		return Action->Execute(Target, Args);
	}

	return false;
}