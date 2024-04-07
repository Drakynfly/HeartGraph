// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_MoveNodeProxy.h"
#include "BloodContainer.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGuids.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_MoveNodeProxy)

const FLazyName UHeartAction_MoveNodeProxy::LocationStorage("nodelocs");

FHeartEvent UHeartAction_MoveNodeProxy::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation,
													   UObject* ContextObject, FBloodContainer& UndoData) const
{
	checkfSlow(Activation.IsRedoAction(), TEXT("HeartAction_MoveNodeProxy should only be executed as a Redo!"))

	auto&& Locations = UndoData.Get<TMap<FHeartNodeGuid, FHeartMoveNodeProxyLocationPair>>(LocationStorage);

	for (auto NodeLocations : Locations)
	{
		if (auto&& Node = Graph->GetNode(NodeLocations.Key))
		{
			Node->SetLocation(NodeLocations.Value.New);
		}
	}

	return FHeartEvent::Handled;
}

bool UHeartAction_MoveNodeProxy::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	auto&& Graph = CastChecked<UHeartGraph>(Target);

	auto&& Locations = UndoData.Get<TMap<FHeartNodeGuid, FHeartMoveNodeProxyLocationPair>>(LocationStorage);

	for (auto NodeLocations : Locations)
	{
		if (auto&& Node = Graph->GetNode(NodeLocations.Key))
		{
			Node->SetLocation(NodeLocations.Value.Original);
		}
	}

	return true;
}