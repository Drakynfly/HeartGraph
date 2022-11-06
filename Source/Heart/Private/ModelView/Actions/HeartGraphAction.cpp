// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartGraphAction.h"
#include "Model/HeartGraph.h"
#include "ModelView/HeartGraphNode.h"
#include "ModelView/HeartGraphPin.h"

bool UHeartGraphAction::Execute(UObject* Object, const FHeartInputActivation& Activation)
{
	if (auto&& Graph = Cast<UHeartGraph>(Object))
	{
		ExecuteOnGraph(Graph, Activation);
		return true;
	}

	if (auto&& Node = Cast<UHeartGraphNode>(Object))
	{
		ExecuteOnNode(Node, Activation);
		return true;
	}

	if (auto&& Pin = Cast<UHeartGraphPin>(Object))
	{
		ExecuteOnPin(Pin, Activation);
		return true;
	}

	return false;
}

void UHeartGraphActionBlueprintBase::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation)
{
	if (ensure(IsValid(Graph)))
	{
		BP_ExecuteOnGraph(Graph, Activation);
	}
}

void UHeartGraphActionBlueprintBase::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation)
{
	if (ensure(IsValid(Node)))
	{
		BP_ExecuteOnNode(Node, Activation);
	}
}

void UHeartGraphActionBlueprintBase::ExecuteOnPin(UHeartGraphPin* Pin, const FHeartInputActivation& Activation)
{
	if (ensure(IsValid(Pin)))
	{
		BP_ExecuteOnPin(Pin, Activation);
	}
}

