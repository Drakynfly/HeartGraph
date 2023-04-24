// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartPinAction_Disconnect.h"

#include "Model/HeartGraphPinInterface.h"

#include "Model/HeartGraphNode.h"

bool UHeartPinAction_Disconnect::CanExecute(const UObject* Object) const
{
	return Object->Implements<UHeartGraphPinInterface>();
}

void UHeartPinAction_Disconnect::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	if (const UHeartGraphNode* Node = Pin->GetNode())
	{
		Node->GetGraph()->DisconnectAllPins({Node->GetGuid(), Pin->GetPinGuid()});
	}
}
