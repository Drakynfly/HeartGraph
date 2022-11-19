// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartGraphCanvasAction.h"
#include "UMG/HeartGraphCanvas.h"
#include "UMG/HeartGraphCanvasNode.h"
#include "UMG/HeartGraphCanvasPin.h"

bool UHeartGraphCanvasAction::Execute(UObject* Object, const FHeartInputActivation& Activation)
{
	if (auto&& Graph = Cast<UHeartGraphCanvas>(Object))
	{
		ExecuteOnGraph(Graph, Activation);
		return true;
	}

	if (auto&& Node = Cast<UHeartGraphCanvasNode>(Object))
	{
		ExecuteOnNode(Node, Activation);
		return true;
	}

	if (auto&& Pin = Cast<UHeartGraphCanvasPin>(Object))
	{
		ExecuteOnPin(Pin, Activation);
		return true;
	}

	return false;
}

void UHeartGraphCanvasActionBlueprintBase::ExecuteOnGraph(UHeartGraphCanvas* Graph, const FHeartInputActivation& Activation)
{
	if (ensure(IsValid(Graph)))
	{
		BP_ExecuteOnGraph(Graph, Activation);
	}
}

void UHeartGraphCanvasActionBlueprintBase::ExecuteOnNode(UHeartGraphCanvasNode* Node, const FHeartInputActivation& Activation)
{
	if (ensure(IsValid(Node)))
	{
		BP_ExecuteOnNode(Node, Activation);
	}
}

void UHeartGraphCanvasActionBlueprintBase::ExecuteOnPin(UHeartGraphCanvasPin* Pin, const FHeartInputActivation& Activation)
{
	if (ensure(IsValid(Pin)))
	{
		BP_ExecuteOnPin(Pin, Activation);
	}
}

