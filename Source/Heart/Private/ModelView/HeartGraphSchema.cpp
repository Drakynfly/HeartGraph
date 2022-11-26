// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartGraphSchema.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPin.h"

bool UHeartGraphSchema::TryConnectPins_Implementation(UHeartGraphPin* PinA, UHeartGraphPin* PinB) const
{
	const FHeartConnectPinsResponse Response = CanPinsConnect(PinA, PinB);

	bool bModified = false;

	switch (Response.Response)
	{
	case EHeartCanConnectPinsResponse::Allow:
		PinA->ConnectTo(PinB);
		bModified = true;
		break;

	case EHeartCanConnectPinsResponse::AllowBreakA:
		PinA->DisconnectFromAll(true);
		PinA->ConnectTo(PinB);
		bModified = true;
		break;

	case EHeartCanConnectPinsResponse::AllowBreakB:
		PinB->DisconnectFromAll(true);
		PinA->ConnectTo(PinB);
		bModified = true;
		break;

	case EHeartCanConnectPinsResponse::AllowBreakAB:
		PinA->DisconnectFromAll(true);
		PinB->DisconnectFromAll(true);
		PinA->ConnectTo(PinB);
		bModified = true;
		break;

	/**
	case CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE:
		bModified = CreateAutomaticConversionNodeAndConnections(PinA, PinB);
		break;

	case CONNECT_RESPONSE_MAKE_WITH_PROMOTION:
		bModified = CreatePromotedConnection(PinA, PinB);
		break;
	*/

	case EHeartCanConnectPinsResponse::Disallow:
	default:
		break;
	}

	if (bModified)
	{
		PinA->GetNode()->NotifyPinConnectionsChanged(PinA);
		PinB->GetNode()->NotifyPinConnectionsChanged(PinB);
	}

	return bModified;
}
FHeartConnectPinsResponse UHeartGraphSchema::CanPinsConnect_Implementation(UHeartGraphPin* A, UHeartGraphPin* B) const
{
	return FHeartConnectPinsResponse{EHeartCanConnectPinsResponse::Allow};
}

UObject* UHeartGraphSchema::GetConnectionVisualizer() const
{
	if (auto&& Class = GetConnectionVisualizerClass())
	{
		return Class->GetDefaultObject();
	}

	return nullptr;
}

UClass* UHeartGraphSchema::GetConnectionVisualizerClass_Implementation() const
{
	return nullptr;
}
