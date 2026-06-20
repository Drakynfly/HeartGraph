// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartHistoryActions.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphPinInterface.h"
#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartHistoryActions)

FHeartEvent UHeartUndoAction::ExecuteOnGraph(const TNotNull<UHeartGraph*> Graph, const FHeartInputActivation& Activation,
											 UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (Heart::Action::History::TryUndo(Graph))
	{
		return FHeartEvent::Handled;
	}
	return FHeartEvent::Failed;
}

FHeartEvent UHeartUndoAction::ExecuteOnNode(const TNotNull<UHeartGraph*> Graph, const FHeartNodeGuid& Node, const FHeartInputActivation& Activation,
											UObject* ContextObject, FBloodContainer& UndoData) const
{
	if (Heart::Action::History::TryUndo(Graph))
	{
		return FHeartEvent::Handled;
	}
	return FHeartEvent::Failed;
}

FHeartEvent UHeartUndoAction::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin,
										   const FHeartInputActivation& Activation, UObject* ContextObject,
										   FBloodContainer& UndoData) const
{
	if (UHeartGraph* Graph = Pin->GetHeartGraph())
	{
		if (Heart::Action::History::TryUndo(Graph))
		{
			return FHeartEvent::Handled;
		}
	}

	return FHeartEvent::Failed;
}

FHeartEvent UHeartRedoAction::ExecuteOnGraph(const TNotNull<UHeartGraph*> Graph, const FHeartInputActivation& Activation,
											 UObject* ContextObject, FBloodContainer& UndoData) const
{
	return Heart::Action::History::TryRedo(Graph);
}

FHeartEvent UHeartRedoAction::ExecuteOnNode(const TNotNull<UHeartGraph*> Graph, const FHeartNodeGuid& Node, const FHeartInputActivation& Activation,
											UObject* ContextObject, FBloodContainer& UndoData) const
{
	return Heart::Action::History::TryRedo(Graph);
}

FHeartEvent UHeartRedoAction::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin,
										   const FHeartInputActivation& Activation, UObject* ContextObject,
										   FBloodContainer& UndoData) const
{
	if (UHeartGraph* Graph = Pin->GetHeartGraph())
	{
		return Heart::Action::History::TryRedo(Graph);
	}
	return FHeartEvent::Failed;
}