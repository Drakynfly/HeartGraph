// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_PinsEditProxy.h"
#include "BloodContainer.h"
#include "Model/HeartGraph.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_PinsEditProxy)

const FLazyName UHeartAction_PinsEditProxy::MementosStorage("proxy_mementos");

FHeartEvent UHeartAction_PinsEditProxy::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation,
													   UObject* ContextObject, FBloodContainer& UndoData) const
{
	checkfSlow(Activation.IsRedoAction(), TEXT("UHeartAction_PinsEditProxy should only be executed as a Redo!"))

	auto&& MementoData = UndoData.Get<FHeartPinsEditProxyUndoData>(MementosStorage);

	Graph->EditConnections().RestoreMementos(MementoData.New);

	return FHeartEvent::Handled;
}

bool UHeartAction_PinsEditProxy::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	auto&& MementoData = UndoData.Get<FHeartPinsEditProxyUndoData>(MementosStorage);

	UHeartGraph* Graph = CastChecked<UHeartGraph>(Target);

	Graph->EditConnections().RestoreMementos(MementoData.Original);

	return true;
}