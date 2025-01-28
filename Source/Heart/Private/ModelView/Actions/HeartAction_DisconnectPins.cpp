// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_DisconnectPins.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphPinInterface.h"

#include "Model/HeartGraphNode.h"
#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_DisconnectPins)

static const FLazyName DisconnectPinsStorage("discpins");

bool FHeartDisconnectPinsUndoData::Serialize(FArchive& Ar)
{
	return !(Ar << TargetNode << Mementos).IsError();
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
	UHeartGraphNode* Node = Pin->GetHeartGraphNode();
	if (!IsValid(Node))
	{
		return FHeartEvent::Failed;
	}

	const FHeartGraphPinReference PinRef{Node->GetGuid(), Pin->GetPinGuid()};

	if (Heart::Action::History::IsUndoable())
	{
		FHeartDisconnectPinsUndoData Data;
		Data.TargetNode = Node;
		Heart::API::FPinEdit(Node).CreateMementos(PinRef, Data.Mementos).DisconnectAll(PinRef);
		UndoData.Add(DisconnectPinsStorage, Data);
	}
	else
	{
		// Quick path when not undoable; don't bother caching mementos
		Heart::API::FPinEdit(Node).DisconnectAll(PinRef);
	}

	return FHeartEvent::Handled;
}

FHeartEvent UHeartAction_DisconnectPins::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation,
													   UObject* ContextObject, FBloodContainer& UndoData) const
{
	const FHeartNodeGuid& Guid = Node->GetGuid();

	if (Heart::Action::History::IsUndoable())
	{
		FHeartDisconnectPinsUndoData Data;
		Data.TargetNode = Node;
		Heart::API::FPinEdit(Node).CreateAllMementos(Guid, Data.Mementos).DisconnectAll(Guid);
		UndoData.Add(DisconnectPinsStorage, Data);
	}
	else
	{
		// Quick path when not undoable; don't bother caching mementos
		Heart::API::FPinEdit(Node).DisconnectAll(Guid);
	}

	return FHeartEvent::Handled;
}

bool UHeartAction_DisconnectPins::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	auto&& Data = UndoData.Get<FHeartDisconnectPinsUndoData>(DisconnectPinsStorage);

	if (!Data.TargetNode.IsValid())
	{
		return false;
	}

	Heart::API::FPinEdit(Data.TargetNode.Get()).RestoreMementos(Data.Mementos);

	return true;
}