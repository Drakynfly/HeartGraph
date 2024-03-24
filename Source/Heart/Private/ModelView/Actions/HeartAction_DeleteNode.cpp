// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_DeleteNode.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNodeInterface.h"
#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_DeleteNode)

bool UHeartAction_DeleteNode::CanExecute(const UObject* Object) const
{
	if (Object->Implements<UHeartGraphNodeInterface>())
	{
		return Cast<IHeartGraphNodeInterface>(Object)->GetHeartGraphNode()->CanDelete();
	}
	return false;
}

void UHeartAction_DeleteNode::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation,
	UObject* ContextObject)
{
	if (Heart::Action::History::IsUndoable())
	{
		// Cache undo data
		DeletedNode = Node;
		Node->GetGraph()->EditConnections().CreateAllMementos(Node->GetGuid(), Mementos);
	}

	Node->GetGraph()->RemoveNode(Node->GetGuid());
}

bool UHeartAction_DeleteNode::Undo(UObject* Target)
{
	if (!IsValid(DeletedNode))
	{
		return false;
	}

	// The graph is still its outer, even tho it was deleted.
	UHeartGraph* Graph = DeletedNode->GetGraph();

	Graph->AddNode(DeletedNode);

	// Relink broken connections
	Graph->EditConnections().RestoreMementos(Mementos);

	return true;
}