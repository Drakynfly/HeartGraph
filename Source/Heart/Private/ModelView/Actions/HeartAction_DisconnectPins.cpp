// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_DisconnectPins.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphPinInterface.h"

#include "Model/HeartGraphNode.h"
#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_DisconnectPins)

bool UHeartAction_DisconnectPins::CanExecute(const UObject* Object) const
{
	return Object->Implements<UHeartGraphPinInterface>() ||
			Object->Implements<UHeartGraphNodeInterface>();
}

FHeartEvent UHeartAction_DisconnectPins::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin,
											   const FHeartInputActivation& Activation, UObject* ContextObject)
{
	UHeartGraphNode* Node = Pin->GetHeartGraphNode();
	if (!IsValid(Node))
	{
		return FHeartEvent::Failed;
	}

	const FHeartGraphPinReference PinRef{Node->GetGuid(), Pin->GetPinGuid()};

	if (Heart::Action::History::IsUndoable())
	{
		TargetNode = Node;
		Node->GetGraph()->EditConnections().CreateMementos(PinRef, Mementos).DisconnectAll(PinRef);
	}
	else
	{
		// Quick path when not undoable; don't bother caching mementos
		Node->GetGraph()->EditConnections().DisconnectAll(PinRef);
	}

	return FHeartEvent::Handled;
}

FHeartEvent UHeartAction_DisconnectPins::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation,
												UObject* ContextObject)
{
	const FHeartNodeGuid Guid = Node->GetGuid();

	if (Heart::Action::History::IsUndoable())
	{
		TargetNode = Node;
		Node->GetGraph()->EditConnections().CreateAllMementos(Guid, Mementos).DisconnectAll(Guid);
	}
	else
	{
		// Quick path when not undoable; don't bother caching mementos
		Node->GetGraph()->EditConnections().DisconnectAll(Guid);
	}

	return FHeartEvent::Handled;
}

bool UHeartAction_DisconnectPins::Undo(UObject* Target)
{
	if (TargetNode.IsValid())
	{
		TargetNode->GetGraph()->EditConnections().RestoreMementos(Mementos);
		return true;
	}

	return false;
}