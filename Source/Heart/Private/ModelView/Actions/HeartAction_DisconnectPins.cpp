// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_DisconnectPins.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphPinInterface.h"

#include "Model/HeartGraphNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_DisconnectPins)

bool UHeartAction_DisconnectPins::CanExecute(const UObject* Object) const
{
	return Object->Implements<UHeartGraphPinInterface>() ||
			Object->IsA<UHeartGraphNode>();
}

void UHeartAction_DisconnectPins::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin,
											   const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (const UHeartGraphNode* Node = Pin->GetHeartGraphNode())
	{
		Node->GetGraph()->EditConnections().DisconnectAll({Node->GetGuid(), Pin->GetPinGuid()});
	}
}

void UHeartAction_DisconnectPins::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation,
												UObject* ContextObject)
{
	Node->GetGraph()->EditConnections().DisconnectAll(Node->GetGuid());
}