// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartGraphAction.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPinInterface.h"
#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphAction)

bool UHeartGraphAction::Execute(const Heart::Action::FArguments& Arguments)
{
	return Heart::Action::History::Log(this, Arguments,
		[&]()
		{
			if (auto&& Graph = Cast<UHeartGraph>(Arguments.Target))
			{
				ExecuteOnGraph(Graph, Arguments.Activation, Arguments.Payload);
				return true;
			}

			if (auto&& Node = Cast<UHeartGraphNode>(Arguments.Target))
			{
				ExecuteOnNode(Node, Arguments.Activation, Arguments.Payload);
				return true;
			}

			if (Arguments.Target->Implements<UHeartGraphPinInterface>())
			{
				ExecuteOnPin(Arguments.Target, Arguments.Activation, Arguments.Payload);
				return true;
			}

			return false;
		});
}

FText UHeartGraphAction_BlueprintBase::GetDescription(const UObject* Target) const
{
	return BP_GetDescription(Target);
}

bool UHeartGraphAction_BlueprintBase::CanExecute(const UObject* Target) const
{
	if (GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BP_CanExecuteOnObject)))
	{
		return BP_CanExecuteOnObject(Target);
	}
	return true;
}

void UHeartGraphAction_BlueprintBase::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (ensure(IsValid(Graph)))
	{
		BP_ExecuteOnGraph(Graph, Activation, ContextObject);
	}
}

void UHeartGraphAction_BlueprintBase::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (ensure(IsValid(Node)))
	{
		BP_ExecuteOnNode(Node, Activation, ContextObject);
	}
}

void UHeartGraphAction_BlueprintBase::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (ensure(IsValid(Pin.GetObject())))
	{
		BP_ExecuteOnPin(Pin, Activation, ContextObject);
	}
}

bool UHeartGraphAction_BlueprintBase::CanUndo(UObject* Target) const
{
	// A Blueprint action can undo if it has an implementation of BP_Undo
	return GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BP_Undo));
}

bool UHeartGraphAction_BlueprintBase::Undo(UObject* Target)
{
	BP_Undo(Target);
	return true;
}