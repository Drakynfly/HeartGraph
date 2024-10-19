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

FHeartEvent UHeartAction_MoveNodeProxy::ExecuteOnGraph(IHeartGraphInterface* Graph, const FHeartInputActivation& Activation,
													   UObject* ContextObject, FBloodContainer& UndoData) const
{
	checkfSlow(Activation.IsRedoAction(), TEXT("HeartAction_MoveNodeProxy should only be executed as a Redo!"))

	auto&& LocationData = UndoData.Get<FHeartMoveNodeProxyUndoData>(LocationStorage);

	TSet<FHeartNodeGuid> Touched;

	for (auto&& NodeLocations : LocationData.Locations)
	{
		Touched.Add(NodeLocations.Key);
		Graph->SetNodeLocation(NodeLocations.Key, NodeLocations.Value.New, false);
	}

	Graph->GetHeartGraph()->NotifyNodeLocationsChanged(Touched, false);

	return FHeartEvent::Handled;
}

bool UHeartAction_MoveNodeProxy::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	IHeartGraphInterface* Interface = Cast<IHeartGraphInterface>(Target);

	auto&& LocationData = UndoData.Get<FHeartMoveNodeProxyUndoData>(LocationStorage);

	TSet<FHeartNodeGuid> Touched;

	for (auto&& NodeLocations : LocationData.Locations)
	{
		Touched.Add(NodeLocations.Key);
		Interface->SetNodeLocation(NodeLocations.Key, NodeLocations.Value.Original, false);
	}

	Interface->GetHeartGraph()->NotifyNodeLocationsChanged(Touched, false);

	return true;
}

#undef LOCTEXT_NAMESPACE