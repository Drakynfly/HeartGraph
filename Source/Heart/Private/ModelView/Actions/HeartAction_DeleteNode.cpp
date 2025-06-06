﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_DeleteNode.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNodeInterface.h"
#include "ModelView/HeartActionHistory.h"
#include "Providers/FlakesBinarySerializer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_DeleteNode)

static const FLazyName DeletedNodeStorage("delnode");

bool FHeartDeleteNodeUndoData::Serialize(FArchive& Ar)
{
	FFlake NodeData;

	if (Ar.IsSaving())
	{
		// Scrunch the node into binary
		NodeData = Flakes::MakeFlake<Flakes::Binary::Type>(DeletedNode);
	}

	Ar << NodeData;

	if (Ar.IsLoading())
	{
		// Restore the node from binary; outer is temporarily the TransientPackage, until it's renamed by ::Undo
		DeletedNode = Flakes::CreateObject<UHeartGraphNode, Flakes::Binary::Type>(NodeData);
	}

	Ar << Mementos;

	return true;
}

bool UHeartAction_DeleteNode::CanExecute(const UObject* Object) const
{
	if (Object->Implements<UHeartGraphNodeInterface>())
	{
		return Cast<IHeartGraphNodeInterface>(Object)->GetHeartGraphNode()->CanDelete();
	}
	return false;
}

FHeartEvent UHeartAction_DeleteNode::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation,
												   UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (Heart::Action::History::IsUndoable())
	{
		// Cache undo data
		FHeartDeleteNodeUndoData Data;
		Data.DeletedNode = Node;

		Heart::API::FPinEdit(Node).CreateAllMementos(Node->GetGuid(), Data.Mementos);

		UndoData.Add(DeletedNodeStorage, Data);
	}

	Node->GetGraph()->RemoveNode(Node->GetGuid());

	return FHeartEvent::Handled;
}

bool UHeartAction_DeleteNode::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	// Target may or may not be valid at this point, so just ignore it.

	auto&& Data = UndoData.Get<FHeartDeleteNodeUndoData>(DeletedNodeStorage);

	if (!IsValid(Data.DeletedNode))
	{
		return false;
	}

	UHeartGraph* Graph = Heart::Action::History::GetGraphFromActionStack();

	// Ensure that the node is reconstructed with the correct graph outer.
	Data.DeletedNode->Rename(nullptr, Graph);

	Graph->AddNode(Data.DeletedNode);

	// Relink broken connections
	Heart::API::FPinEdit(Graph).RestoreMementos(Data.Mementos);

	return true;
}