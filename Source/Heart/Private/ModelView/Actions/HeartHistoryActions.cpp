// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartHistoryActions.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPinInterface.h"
#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartHistoryActions)

FHeartEvent UHeartUndoAction::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation,
												  UObject* ContextObject)
{
	if (Heart::Action::History::TryUndo(Graph))
	{
		return FHeartEvent::Handled;
	}
	return FHeartEvent::Failed;
}

FHeartEvent UHeartUndoAction::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation,
												 UObject* ContextObject)
{
	if (Heart::Action::History::TryUndo(Node->GetGraph()))
	{
		return FHeartEvent::Handled;
	}
	return FHeartEvent::Failed;
}

FHeartEvent UHeartUndoAction::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin,
												const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (Heart::Action::History::TryUndo(Pin->GetHeartGraphNode()->GetGraph()))
	{
		return FHeartEvent::Handled;
	}
	return FHeartEvent::Failed;
}

FHeartEvent UHeartRedoAction::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation,
												  UObject* ContextObject)
{
	return Heart::Action::History::TryRedo(Graph);
}

FHeartEvent UHeartRedoAction::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation,
	UObject* ContextObject)
{
	return Heart::Action::History::TryRedo(Node->GetGraph());
}

FHeartEvent UHeartRedoAction::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin,
	const FHeartInputActivation& Activation, UObject* ContextObject)
{
	return Heart::Action::History::TryRedo(Pin->GetHeartGraphNode()->GetGraph());
}