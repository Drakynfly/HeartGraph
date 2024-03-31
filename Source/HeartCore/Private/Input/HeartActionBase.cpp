// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartActionBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartActionBase)

bool UHeartActionBase::QuickExecuteGraphAction(const TSubclassOf<UHeartActionBase> Class,
													UObject* Target, const FHeartManualEvent& Activation)
{
	if (!ensure(IsValid(Class)))
	{
		return false;
	}

	if (!Class->GetDefaultObject<UHeartActionBase>()->CanExecute(Target))
	{
		return false;
	}

	auto&& Action = CreateGraphAction(Class);

	Heart::Action::FArguments Args;
	Args.Target = Target;
	Args.Activation = Activation;

	return Action->Execute(Args);
}

bool UHeartActionBase::QuickExecuteGraphActionWithPayload(const TSubclassOf<UHeartActionBase> Class,
                                                               UObject* Target, const FHeartManualEvent& Activation, UObject* Payload)
{
	if (!ensure(IsValid(Class)))
	{
		return false;
	}

	if (!Class->GetDefaultObject<UHeartActionBase>()->CanExecute(Target))
	{
		return false;
	}

	auto&& Action = CreateGraphAction(Class);

	Heart::Action::FArguments Args;
	Args.Target = Target;
	Args.Activation = Activation;
	Args.Payload = Payload;

	return Action->Execute(Args);
}

UHeartActionBase* UHeartActionBase::CreateGraphAction(const TSubclassOf<UHeartActionBase> Class)
{
	if (!ensure(IsValid(Class)))
	{
		return nullptr;
	}

	return NewObject<UHeartActionBase>(GetTransientPackage(), Class);
}

bool UHeartActionBase::ExecuteGraphAction(UHeartActionBase* Action, UObject* Target, const FHeartManualEvent& Activation)
{
	if (ensure(Action))
	{
		if (!Action->CanExecute(Target))
		{
			return false;
		}

		Heart::Action::FArguments Args;
		Args.Target = Target;
		Args.Activation = Activation;

		return Action->Execute(Args);
	}

	return false;
}