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
	TSoftObjectPtr<> NodeOuter = DeletedNode.GetOuter();

	if (Ar.IsSaving())
	{
		// Scrunch the node into binary
		NodeData = Heart::Flakes::CreateFlake(DeletedNode);
	}

	Ar << NodeOuter;
	Ar << NodeData;

	if (Ar.IsLoading() && NodeOuter.IsValid())
	{
		// Restore the node from binary
		DeletedNode = Heart::Flakes::CreateObject<UHeartGraphNode>(NodeData, NodeOuter.Get());
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

	// The graph is still its outer, even tho it was deleted.
	UHeartGraph* Graph = Data.DeletedNode->GetGraph();

	Graph->AddNode(Data.DeletedNode);

	// Relink broken connections
	Graph->EditConnections().RestoreMementos(Data.Mementos);

	return true;
}