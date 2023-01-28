// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartGraphActionBase.h"

bool UHeartGraphActionBase::QuickExecuteGraphAction(const TSubclassOf<UHeartGraphActionBase> Class,
	UObject* Target, const FHeartInputActivation& Activation)
{
	if (!ensure(IsValid(Class)))
	{
		return false;
	}

	auto&& Action = CreateGraphAction(Class);
	return Action->Execute(Target, Activation);
}

bool UHeartGraphActionBase::QuickExecuteGraphActionWithPayload(const TSubclassOf<UHeartGraphActionBase> Class,
                                                               UObject* Target, const FHeartInputActivation& Activation, UObject* Payload)
{
	if (!ensure(IsValid(Class)))
	{
		return false;
	}

	auto&& Action = CreateGraphAction(Class);
	return Action->Execute(Target, Activation, Payload);
}

UHeartGraphActionBase* UHeartGraphActionBase::CreateGraphAction(const TSubclassOf<UHeartGraphActionBase> Class)
{
	if (!ensure(IsValid(Class)))
	{
		return nullptr;
	}

	return NewObject<UHeartGraphActionBase>(GetTransientPackage(), Class);
}

bool UHeartGraphActionBase::ExecuteGraphAction(UHeartGraphActionBase* Action, UObject* Target,
	const FHeartInputActivation& Activation)
{
	if (ensure(Action))
	{
		return Action->Execute(Target, Activation);
	}

	return false;
}

bool UHeartGraphActionBase::Execute(UObject* Object, const FHeartInputActivation& Activation)
{
	return Execute(Object, Activation, nullptr);
}
