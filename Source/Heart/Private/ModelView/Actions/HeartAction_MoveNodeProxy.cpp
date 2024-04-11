// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_MoveNodeProxy.h"
#include "BloodContainer.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_MoveNodeProxy)

#define LOCTEXT_NAMESPACE "HeartAction_MoveNodeProxy"

const FLazyName UHeartAction_MoveNodeProxy::LocationStorage("nodelocs");

FText UHeartAction_MoveNodeProxy::GetDescription(const UObject* Target) const
{
	return LOCTEXT("Description", "Move Node Helper");
}

FHeartEvent UHeartAction_MoveNodeProxy::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation,
													   UObject* ContextObject, FBloodContainer& UndoData) const
{
	checkfSlow(Activation.IsRedoAction(), TEXT("HeartAction_MoveNodeProxy should only be executed as a Redo!"))

	auto&& LocationData = UndoData.Get<FHeartMoveNodeProxyUndoData>(LocationStorage);

	for (auto NodeLocations : LocationData.Locations)
	{
		if (auto&& Node = Graph->GetNode(NodeLocations.Key))
		{
			Node->SetLocation(NodeLocations.Value.New);
			Node->GetGraph()->NotifyNodeLocationsChanged({Node}, false);
		}
	}

	return FHeartEvent::Handled;
}

bool UHeartAction_MoveNodeProxy::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	auto&& Graph = CastChecked<UHeartGraph>(Target);

	auto&& LocationData = UndoData.Get<FHeartMoveNodeProxyUndoData>(LocationStorage);

	for (auto NodeLocations : LocationData.Locations)
	{
		if (auto&& Node = Graph->GetNode(NodeLocations.Key))
		{
			Node->SetLocation(NodeLocations.Value.Original);
			Node->GetGraph()->NotifyNodeLocationsChanged({Node}, false);
		}
	}

	return true;
}

#undef LOCTEXT_NAMESPACE