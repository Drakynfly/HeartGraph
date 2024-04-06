// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_DeleteNode.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNodeInterface.h"
#include "ModelView/HeartActionHistory.h"
#include "Serialization/HeartFlakes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_DeleteNode)

static const FLazyName DeletedNodeStorage("delnode");

bool FHeartDeleteNodeUndoData::Serialize(FArchive& Ar)
{
	FHeartFlake NodeData;

	if (Ar.IsSaving())
	{
		// Scrunch the node into binary
		NodeData = Heart::Flakes::CreateFlake(DeletedNode);
	}

	Ar << NodeData;

	if (Ar.IsLoading())
	{
		// Restore the node from binary; outer is temporarily the TransientPackage, until it's renamed by ::Undo
		DeletedNode = Heart::Flakes::CreateObject<UHeartGraphNode>(NodeData);
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

		Node->GetGraph()->EditConnections().CreateAllMementos(Node->GetGuid(), Data.Mementos);

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
	Graph->EditConnections().RestoreMementos(Data.Mementos);

	return true;
}