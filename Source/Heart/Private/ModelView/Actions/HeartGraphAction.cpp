// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartGraphAction.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPinInterface.h"

bool UHeartGraphAction::Execute(UObject* Object, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (auto&& Graph = Cast<UHeartGraph>(Object))
	{
		ExecuteOnGraph(Graph, Activation, ContextObject);
		return true;
	}

	if (auto&& Node = Cast<UHeartGraphNode>(Object))
	{
		ExecuteOnNode(Node, Activation, ContextObject);
		return true;
	}

	if (Object->Implements<UHeartGraphPinInterface>())
	{
		ExecuteOnPin(Object, Activation, ContextObject);
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
	return BP_CanExecuteOnObject(Object);
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

