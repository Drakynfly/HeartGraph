// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartActionBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartActionBase)

namespace Heart::Action
{
	FText FNativeExec::GetDescription(const UHeartActionBase* Action, const UObject* Target)
	{
		return Action->GetDescription(Target);
	}

	bool FNativeExec::CanExecute(const UHeartActionBase* Action, const UObject* Target)
	{
		return Action->CanExecute(Target);
	}

	FHeartEvent FNativeExec::Execute(const UHeartActionBase* Action, const FArguments& Arguments)
	{
		return Action->Execute(Arguments);
	}

	bool FNativeExec::CanUndo(const UHeartActionBase* Action, const UObject* Target)
	{
		return Action->CanUndo(Target);
	}

	bool FNativeExec::Undo(const UHeartActionBase* Action, UObject* Target, const FBloodContainer& UndoData)
	{
		return Action->Undo(Target, UndoData);
	}

	FText GetDescription(const TSubclassOf<UHeartActionBase> Action, const UObject* Target)
	{
		return IsValid(Action) ?
			FNativeExec::GetDescription(Action->GetDefaultObject<UHeartActionBase>(), Target) :
			FText::GetEmpty();
	}

	bool CanExecute(const TSubclassOf<UHeartActionBase> Action, const UObject* Target)
	{
		return IsValid(Action) ?
			FNativeExec::CanExecute(Action->GetDefaultObject<UHeartActionBase>(), Target) :
			false;
	}

	FHeartEvent Execute(const TSubclassOf<UHeartActionBase> Action, UObject* Target,
						const FHeartInputActivation& Activation, UObject* Payload)
	{
		if (!ensure(IsValid(Action)))
		{
			return FHeartEvent::Invalid;
		}

		const UHeartActionBase* ActionObject = GetDefault<UHeartActionBase>(Action);

		if (!FNativeExec::CanExecute(ActionObject, Target))
		{
			return FHeartEvent::Invalid;
		}

		FArguments Args;
		Args.Target = Target;
		Args.Activation = Activation;
		Args.Payload = Payload;

		return FNativeExec::Execute(ActionObject, Args);
	}

	bool CanUndo(const TSubclassOf<UHeartActionBase> Action, const UObject* Target)
	{
		return IsValid(Action) ?
			FNativeExec::CanUndo(Action->GetDefaultObject<UHeartActionBase>(), Target) :
			false;
	}

	bool Undo(const TSubclassOf<UHeartActionBase> Action, UObject* Target, const FBloodContainer& UndoData)
	{
		if (!ensure(IsValid(Action)))
		{
			return false;
		}

		const UHeartActionBase* ActionObject = GetDefault<UHeartActionBase>(Action);

		return FNativeExec::Undo(ActionObject, Target, UndoData);
	}
}

void UHeartActionBase::PostInitProperties()
{
	Super::PostInitProperties();
	ensureMsgf(IsTemplate(),
		TEXT("Heart Actions should not be created via NewObject. They are const classes that have all their member functions called by statics!"));
}

FText UHeartActionBase::GetActionDescription(const TSubclassOf<UHeartActionBase> Class, const UObject* Target)
{
	return Heart::Action::GetDescription(Class, Target);
}

bool UHeartActionBase::CanExecute(const TSubclassOf<UHeartActionBase> Class, const UObject* Target)
{
	return Heart::Action::CanExecute(Class, Target);
}

FHeartEvent UHeartActionBase::ExecuteGraphAction(const TSubclassOf<UHeartActionBase> Class,
													  UObject* Target, const FHeartManualEvent& Activation)
{
	return Heart::Action::Execute(Class, Target, Activation);
}

FHeartEvent UHeartActionBase::ExecuteGraphActionWithPayload(const TSubclassOf<UHeartActionBase> Class,
															   UObject* Target, const FHeartManualEvent& Activation, UObject* Payload)
{
	return Heart::Action::Execute(Class, Target, Activation, Payload);
}

FText UHeartActionBase::GetDescription(const UObject* Target) const
{
	if (!IsValid(Target))
	{
		return FText::FromString(GetClass()->GetName());
	}

	return FText::FromString(FString::Printf(TEXT("%s:'%s'"),
		*GetClass()->GetName(), *Target->GetName()));
}