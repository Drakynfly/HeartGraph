// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartPinAction_Disconnect.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphPinInterface.h"

#include "Model/HeartGraphNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartPinAction_Disconnect)

bool UHeartPinAction_Disconnect::CanExecute(const UObject* Object) const
{
	return Object->Implements<UHeartGraphPinInterface>();
}

void UHeartPinAction_Disconnect::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (const UHeartGraphNode* Node = Pin->GetHeartGraphNode())
	{
		Node->GetGraph()->DisconnectAllPins({Node->GetGuid(), Pin->GetPinGuid()});
	}
}

bool UHeartNodeAction_Disconnect::CanExecute(const UObject* Object) const
{
	return Object->Implements<UHeartGraphNodeInterface>();
}

void UHeartNodeAction_Disconnect::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation,
	UObject* ContextObject)
{
	if (IsValid(Node))
	{
		Node->GetGraph()->EditConnections().DisconnectAll(Node->GetGuid());
	}
}