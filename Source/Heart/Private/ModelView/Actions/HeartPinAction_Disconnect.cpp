// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartPinAction_Disconnect.h"
#include "Model/HeartGraphPin.h"

void UHeartPinAction_Disconnect::ExecuteOnPin(UHeartGraphPin* Pin, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	Pin->DisconnectFromAll(true);
}
