// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartActionBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartActionBase)

FHeartEvent UHeartActionBase::QuickExecuteGraphAction(const TSubclassOf<UHeartActionBase> Class,
													UObject* Target, const FHeartManualEvent& Activation)
{
	if (!ensure(IsValid(Class)))
	{
		return FHeartEvent::Invalid;
	}

	if (!Class->GetDefaultObject<UHeartActionBase>()->CanExecute(Target))
	{
		return FHeartEvent::Invalid;
	}

	auto&& Action = CreateGraphAction(Class);

	Heart::Action::FArguments Args;
	Args.Target = Target;
	Args.Activation = Activation;

	return Action->Execute(Args);
}

FHeartEvent UHeartActionBase::QuickExecuteGraphActionWithPayload(const TSubclassOf<UHeartActionBase> Class,
                                                               UObject* Target, const FHeartManualEvent& Activation, UObject* Payload)
{
	if (!ensure(IsValid(Class)))
	{
		return FHeartEvent::Invalid;
	}

	if (!Class->GetDefaultObject<UHeartActionBase>()->CanExecute(Target))
	{
		return FHeartEvent::Invalid;
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

FHeartEvent UHeartActionBase::ExecuteGraphAction(UHeartActionBase* Action, UObject* Target, const FHeartManualEvent& Activation)
{
	if (!ensure(IsValid(Action)))
	{
		return FHeartEvent::Invalid;
	}

	if (!Action->CanExecute(Target))
	{
		return FHeartEvent::Invalid;
	}

	Heart::Action::FArguments Args;
	Args.Target = Target;
	Args.Activation = Activation;

	return Action->Execute(Args);
}