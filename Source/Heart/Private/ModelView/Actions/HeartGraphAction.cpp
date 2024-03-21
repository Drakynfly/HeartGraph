// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartGraphAction.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPinInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartGraphAction)

bool UHeartGraphAction::Execute(UObject* Object, const Heart::Action::FArguments& Arguments)
{
	if (auto&& Graph = Cast<UHeartGraph>(Object))
	{
		ExecuteOnGraph(Graph, Arguments.Activation, Arguments.Payload);
		return true;
	}

	if (auto&& Node = Cast<UHeartGraphNode>(Object))
	{
		ExecuteOnNode(Node, Arguments.Activation, Arguments.Payload);
		return true;
	}

	if (Object->Implements<UHeartGraphPinInterface>())
	{
		ExecuteOnPin(Object, Arguments.Activation, Arguments.Payload);
		return true;
	}

	return false;
}

FText UHeartGraphActionBlueprintBase::GetDescription(const UObject* Object) const
{
	return BP_GetDescription(Object);
}

bool UHeartGraphActionBlueprintBase::CanExecute(const UObject* Object) const
{
	if (GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BP_CanExecuteOnObject)))
	{
		return BP_CanExecuteOnObject(Object);
	}
	return true;
}

void UHeartGraphActionBlueprintBase::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (ensure(IsValid(Graph)))
	{
		BP_ExecuteOnGraph(Graph, Activation, ContextObject);
	}
}

void UHeartGraphActionBlueprintBase::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (ensure(IsValid(Node)))
	{
		BP_ExecuteOnNode(Node, Activation, ContextObject);
	}
}

void UHeartGraphActionBlueprintBase::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (ensure(IsValid(Pin.GetObject())))
	{
		BP_ExecuteOnPin(Pin, Activation, ContextObject);
	}
}