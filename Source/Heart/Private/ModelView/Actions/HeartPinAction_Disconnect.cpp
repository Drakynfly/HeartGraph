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
	const UHeartGraphNode* Node = IHeartGraphPinInterface::Execute_GetNode(Pin.GetObject());
	const FHeartGraphPinReference Ref = {Node->GetGuid(), IHeartGraphPinInterface::Execute_GetPinGuid(Pin.GetObject())};
	Node->GetGraph()->DisconnectAllPins(Ref);
}
