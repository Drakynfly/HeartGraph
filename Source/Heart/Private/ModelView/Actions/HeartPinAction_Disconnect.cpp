// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartPinAction_Disconnect.h"
#include "Model/HeartGraphPin.h"

bool UHeartPinAction_Disconnect::CanExecute(const UObject* Object) const
{
	return Object->IsA<UHeartGraphPin>();
}

void UHeartPinAction_Disconnect::ExecuteOnPin(UHeartGraphPin* Pin, const FHeartInputActivation& Activation, UObject* ContextObject)
{
	Pin->DisconnectFromAll(true);
}
