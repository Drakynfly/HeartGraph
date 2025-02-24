﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphProxy/HeartNetClient.h"
#include "LogHeartNet.h"
#include "GraphProxy/HeartGraphNetProxy.h"
#include "GraphProxy/HeartNetReplicationTypes.h"
#include "Input/HeartActionBase.h"
#include "Model/HeartGraphTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNetClient)

UHeartNetClient::UHeartNetClient()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UHeartNetClient::Server_UndoGraphAction_Implementation(UHeartGraphNetProxy* Proxy)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client undoing action."))
	Proxy->ExecuteUndo_Client();
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client undid action."))
}

void UHeartNetClient::Server_RedoGraphAction_Implementation(UHeartGraphNetProxy* Proxy)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client redoing action"))
	Proxy->ExecuteRedo_Client();
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client redid action."))
}

void UHeartNetClient::Server_UpdateGraphNode_Implementation(UHeartGraphNetProxy* Proxy,
															const FHeartReplicatedFlake& NodeFlake, const EHeartUpdateNodeType Type)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client updating node. (%i bytes) Type '%s'"), NodeFlake.Flake.Data.Num(), *StaticEnum<EHeartUpdateNodeType>()->GetValueAsString(Type))
	Proxy->UpdateNodeData_Client(NodeFlake, Type == EHeartUpdateNodeType::NodeObject ? Heart::Net::Tags::Node_ClientUpdateNodeObject : Heart::Net::Tags::Other);
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client updated node."))
}

void UHeartNetClient::Server_OnNodeAdded_Implementation(UHeartGraphNetProxy* Proxy, const FHeartReplicatedFlake& HeartGraphNode)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client adding node. (%i bytes)"), HeartGraphNode.Flake.Data.Num())
	Proxy->OnNodeAdded_Client(HeartGraphNode);
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client added node."))
}

void UHeartNetClient::Server_OnNodesMoved_Implementation(UHeartGraphNetProxy* Proxy,
														 const FHeartNodeMoveEvent_Net& NodeMoveEvent)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client moving node"))
	Proxy->OnNodesMoved_Client(NodeMoveEvent);
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client moved node"))
}

void UHeartNetClient::Server_OnNodeRemoved_Implementation(UHeartGraphNetProxy* Proxy, const FHeartNodeGuid& HeartGraphNode)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client removing node"))
	Proxy->OnNodeRemoved_Client(HeartGraphNode);
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client removed node"))
}

void UHeartNetClient::Server_OnNodeConnectionsChanged_Implementation(UHeartGraphNetProxy* Proxy,
																	 const FHeartGraphConnectionEvent_Net& GraphConnectionEvent)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client changing node connections"))
	Proxy->OnNodeConnectionsChanged_Client(GraphConnectionEvent);
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client changed node connections"))
}

void UHeartNetClient::Server_ExecuteGraphAction_Implementation(UHeartGraphNetProxy* Proxy, const TSubclassOf<UHeartActionBase> Action,
															   const FHeartRemoteGraphActionArguments& Args)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client wants to run graph action '%s'"), *Action->GetName())
	Proxy->ExecuteGraphAction_Client(Action, Args);
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client wants to ran graph action"))
}