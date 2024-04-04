// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartGraphAction.h"
#include "Input/HeartEvent.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPinInterface.h"
#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphAction)

FHeartEvent UHeartGraphAction::Execute(const Heart::Action::FArguments& Arguments) const
{
	return Heart::Action::History::Log(this, Arguments,
		[&](FBloodContainer& UndoData)
		{
			if (!IsValid(Arguments.Target))
			{
				return FHeartEvent::Invalid;
			}

			if (auto&& GraphInterface = Cast<IHeartGraphInterface>(Arguments.Target))
			{
				return ExecuteOnGraph(GraphInterface->GetHeartGraph(), Arguments.Activation, Arguments.Payload, UndoData);
			}
			if (auto&& NodeInterface = Cast<IHeartGraphNodeInterface>(Arguments.Target))
			{
				return ExecuteOnNode(NodeInterface->GetHeartGraphNode(), Arguments.Activation, Arguments.Payload, UndoData);
			}
			if (Arguments.Target->Implements<UHeartGraphPinInterface>())
			{
				return ExecuteOnPin(Arguments.Target, Arguments.Activation, Arguments.Payload, UndoData);
			}

			return FHeartEvent::Ignored;
		});
}

FHeartEvent UHeartGraphAction::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation,
	UObject* ContextObject, FBloodContainer& UndoData) const
{
	return FHeartEvent::Ignored;
}

FHeartEvent UHeartGraphAction::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation,
	UObject* ContextObject, FBloodContainer& UndoData) const
{
	return FHeartEvent::Ignored;
}

FHeartEvent UHeartGraphAction::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin,
	const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	return FHeartEvent::Ignored;
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

bool UHeartGraphAction_BlueprintBase::CanUndo(const UObject* Target) const
{
	// A Blueprint action can undo if it has an implementation of BP_Undo
	return GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BP_Undo));
}

bool UHeartGraphAction_BlueprintBase::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	BP_Undo(Target, UndoData);
	return true;
}

FHeartEvent UHeartGraphAction_BlueprintBase::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BP_ExecuteOnGraph)))
	{
		return BP_ExecuteOnGraph(Graph, Activation, ContextObject, UndoData);
	}
	return FHeartEvent::Ignored;
}

FHeartEvent UHeartGraphAction_BlueprintBase::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BP_ExecuteOnNode)))
	{
		return BP_ExecuteOnNode(Node, Activation, ContextObject, UndoData);
	}
	return FHeartEvent::Ignored;
}

FHeartEvent UHeartGraphAction_BlueprintBase::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BP_ExecuteOnPin)))
	{
		return BP_ExecuteOnPin(Pin, Activation, ContextObject, UndoData);
	}
	return FHeartEvent::Ignored;
}