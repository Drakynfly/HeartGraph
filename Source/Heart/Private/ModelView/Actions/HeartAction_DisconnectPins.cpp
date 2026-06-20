// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_DisconnectPins.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNodeInterface.h"
#include "Model/HeartGraphPinInterface.h"

#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_DisconnectPins)

static constexpr FLazyName DisconnectPinsStorage("discpins");

bool FHeartDisconnectPinsUndoData::Serialize(FArchive& Ar)
{
	return !(Ar << Mementos).IsError();
}

bool UHeartAction_DisconnectPins::CanExecute(const UObject* Object) const
{
	return Object->Implements<UHeartGraphPinInterface>() ||
			Object->Implements<UHeartGraphNodeInterface>();
}

FHeartEvent UHeartAction_DisconnectPins::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin,
													  const FHeartInputActivation& Activation, UObject* ContextObject,
													  FBloodContainer& UndoData) const
{
	UHeartGraph* Graph = Pin->GetHeartGraph();
	if (!IsValid(Graph))
	{
		return FHeartEvent::Failed;
	}

	const FHeartGraphPinReference PinRef{Pin->GetNodeGuid(), Pin->GetPinGuid()};

	if (Heart::Action::History::IsUndoable())
	{
		FHeartDisconnectPinsUndoData Data;
		Heart::API::FPinEdit(Graph).CreateMementos(PinRef, Data.Mementos).DisconnectAll(PinRef);
		UndoData.Add(DisconnectPinsStorage, Data);
	}
	else
	{
		// Quick path when not undoable; don't bother caching mementos
		Heart::API::FPinEdit(Graph).DisconnectAll(PinRef);
	}

	return FHeartEvent::Handled;
}

FHeartEvent UHeartAction_DisconnectPins::ExecuteOnNode(const TNotNull<UHeartGraph*> Graph, const FHeartNodeGuid& Node, const FHeartInputActivation& Activation,
													   UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (Heart::Action::History::IsUndoable())
	{
		FHeartDisconnectPinsUndoData Data;
		Heart::API::FPinEdit(Graph).CreateAllMementos(Node, Data.Mementos).DisconnectAll(Node);
		UndoData.Add(DisconnectPinsStorage, Data);
	}
	else
	{
		// Quick path when not undoable; don't bother caching mementos
		Heart::API::FPinEdit(Graph).DisconnectAll(Node);
	}

	return FHeartEvent::Handled;
}

bool UHeartAction_DisconnectPins::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	auto&& Data = UndoData.Get<FHeartDisconnectPinsUndoData>(DisconnectPinsStorage);

	UHeartGraph* Graph = Heart::Action::History::GetGraphFromActionStack();
	if (!IsValid(Graph))
	{
		return false;
	}

	Heart::API::FPinEdit(Graph).RestoreMementos(Data.Mementos);

	return true;
}