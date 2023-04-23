// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartGraphSchema.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPin.h"

bool UHeartGraphSchema::TryGetWorldForGraph_Implementation(const UHeartGraph* HeartGraph, UWorld*& World) const
{
	return false;
}

UObject* UHeartGraphSchema::GetConnectionVisualizer() const
{
	if (auto&& Class = GetConnectionVisualizerClass())
	{
		return Class->GetDefaultObject();
	}

	return nullptr;
}

bool UHeartGraphSchema::TryConnectPins_Implementation(UHeartGraph* Graph, FHeartGraphPinReference PinA, FHeartGraphPinReference PinB) const
{
	UHeartGraphPin* PinPtrA = Graph->GetNode(PinA.NodeGuid)->GetPin(PinA.PinGuid);
	UHeartGraphPin* PinPtrB = Graph->GetNode(PinB.NodeGuid)->GetPin(PinB.PinGuid);

	const FHeartConnectPinsResponse Response = CanPinsConnect(PinPtrA, PinPtrB);

	bool bModified = false;

	switch (Response.Response)
	{
	case EHeartCanConnectPinsResponse::Allow:
		Graph->ConnectPins(PinA, PinB);
		bModified = true;
		break;

	case EHeartCanConnectPinsResponse::AllowBreakA:
		PinPtrA->DisconnectFromAll(true);
		Graph->ConnectPins(PinA, PinB);
		bModified = true;
		break;

	case EHeartCanConnectPinsResponse::AllowBreakB:
		PinPtrB->DisconnectFromAll(true);
		Graph->ConnectPins(PinA, PinB);
		bModified = true;
		break;

	case EHeartCanConnectPinsResponse::AllowBreakAB:
		PinPtrA->DisconnectFromAll(true);
		PinPtrB->DisconnectFromAll(true);
		Graph->ConnectPins(PinA, PinB);
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
		PinPtrA->GetNode()->NotifyPinConnectionsChanged(PinPtrA);
		PinPtrB->GetNode()->NotifyPinConnectionsChanged(PinPtrB);
		Graph->NotifyNodeConnectionsChanged({PinPtrA->GetNode(), PinPtrB->GetNode()}, {PinPtrA, PinPtrB});
	}

	return bModified;
}
FHeartConnectPinsResponse UHeartGraphSchema::CanPinsConnect_Implementation(UHeartGraphPin* A, UHeartGraphPin* B) const
{
	return FHeartConnectPinsResponse{EHeartCanConnectPinsResponse::Allow};
}

void UHeartGraphSchema::CreateDefaultNodesForGraph_Implementation(UHeartGraph* Graph) const
{
	// Does nothing by default
}

UClass* UHeartGraphSchema::GetConnectionVisualizerClass_Implementation() const
{
	return nullptr;
}
