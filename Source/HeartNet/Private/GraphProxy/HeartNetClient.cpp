// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphProxy/HeartNetClient.h"
#include "LogHeartNet.h"
#include "GraphProxy/HeartGraphNetProxy.h"
#include "Input/HeartActionBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNetClient)

UHeartNetClient::UHeartNetClient()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UHeartNetClient::Server_OnNodeAdded_Implementation(UHeartGraphNetProxy* Proxy, const FHeartReplicatedFlake& HeartGraphNode)
{
	ensure(IsValid(Proxy));
	UE_LOGF(LogHeartNet, Log, "Server: Client adding node. (%llu bytes)", HeartGraphNode.Flake.Data.NumBytes())
	Proxy->OnNodeAdded_Client(HeartGraphNode);
	UE_LOGF(LogHeartNet, Log, "Server: Client added node.")
}

void UHeartNetClient::Server_OnNodesMoved_Implementation(UHeartGraphNetProxy* Proxy,
														 const FHeartNodeMoveEvent_Net& NodeMoveEvent)
{
	ensure(IsValid(Proxy));
	UE_LOGF(LogHeartNet, Log, "Server: Client moving node")
	Proxy->OnNodesMoved_Client(NodeMoveEvent);
	UE_LOGF(LogHeartNet, Log, "Server: Client moved node")
}

void UHeartNetClient::Server_OnNodeRemoved_Implementation(UHeartGraphNetProxy* Proxy, const FHeartNodeGuid& Node)
{
	ensure(IsValid(Proxy));
	UE_LOGF(LogHeartNet, Log, "Server: Client removing node")
	Proxy->OnNodeRemoved_Client(Node);
	UE_LOGF(LogHeartNet, Log, "Server: Client removed node")
}

void UHeartNetClient::Server_OnNodeConnectionsChanged_Implementation(UHeartGraphNetProxy* Proxy,
																	 const FHeartGraphConnectionEvent_Net& GraphConnectionEvent)
{
	ensure(IsValid(Proxy));
	UE_LOGF(LogHeartNet, Log, "Server: Client changing node connections")
	Proxy->OnNodeConnectionsChanged_Client(GraphConnectionEvent);
	UE_LOGF(LogHeartNet, Log, "Server: Client changed node connections")
}

void UHeartNetClient::Server_OnExtensionAdded_Implementation(UHeartGraphNetProxy* Proxy,
	const FHeartReplicatedFlake& ExtensionData)
{
	ensure(IsValid(Proxy));
	UE_LOGF(LogHeartNet, Log, "Server: Client adding extension. (%llu bytes)", ExtensionData.Flake.Data.NumBytes())
	Proxy->OnExtensionAdded_Client(ExtensionData);
	UE_LOGF(LogHeartNet, Log, "Server: Client added extension.")
}

void UHeartNetClient::Server_OnExtensionRemoved_Implementation(UHeartGraphNetProxy* Proxy,
	const FHeartExtensionGuid& Extension)
{
	ensure(IsValid(Proxy));
	UE_LOGF(LogHeartNet, Log, "Server: Client removing extension")
	Proxy->OnExtensionRemoved_Client(Extension);
	UE_LOGF(LogHeartNet, Log, "Server: Client removed extension")
}

void UHeartNetClient::Server_OnNodeComponentAdded_Implementation(UHeartGraphNetProxy* Proxy,
	const FHeartReplicatedNodeComponent& ComponentData)
{
	ensure(IsValid(Proxy));
	UE_LOGF(LogHeartNet, Log, "Server: Client adding node component. (%llu bytes)", ComponentData.Flake.Data.NumBytes())
	Proxy->OnNodeComponentAdded_Client(ComponentData);
	UE_LOGF(LogHeartNet, Log, "Server: Client added node component.")
}

void UHeartNetClient::Server_OnNodeComponentRemoved_Implementation(UHeartGraphNetProxy* Proxy,
	const FHeartNodeGuid& Node, const FHeartExtensionGuid& Component)
{
	ensure(IsValid(Proxy));
	UE_LOGF(LogHeartNet, Log, "Server: Client removing node component")
	Proxy->OnNodeComponentRemoved_Client(Node, Component);
	UE_LOGF(LogHeartNet, Log, "Server: Client removed extension")
}

void UHeartNetClient::Server_UpdateGraphNode_Implementation(UHeartGraphNetProxy* Proxy,
															const FHeartReplicatedFlake& NodeFlake, const EHeartUpdateNodeType Type)
{
	ensure(IsValid(Proxy));
	UE_LOGF(LogHeartNet, Log, "Server: Client updating node. (%llu bytes) Type '%ls'", NodeFlake.Flake.Data.NumBytes(), *StaticEnum<EHeartUpdateNodeType>()->GetValueAsString(Type))
	Proxy->UpdateNodeData_Client(NodeFlake, Type == EHeartUpdateNodeType::NodeObject ? Heart::Net::Tags::Node_ClientUpdateNodeObject : Heart::Net::Tags::Other);
	UE_LOGF(LogHeartNet, Log, "Server: Client updated node.")
}

void UHeartNetClient::Server_ExecuteGraphAction_Implementation(UHeartGraphNetProxy* Proxy, const TSubclassOf<UHeartActionBase> Action,
															   const FHeartRemoteGraphActionArguments& Args)
{
	ensure(IsValid(Proxy));
	UE_LOGF(LogHeartNet, Log, "Server: Client wants to run graph action '%ls'", *Action->GetName())
	Proxy->ExecuteGraphAction_Client(Action, Args);
	UE_LOGF(LogHeartNet, Log, "Server: Client wants to ran graph action")
}

void UHeartNetClient::Server_UndoGraphAction_Implementation(UHeartGraphNetProxy* Proxy)
{
	ensure(IsValid(Proxy));
	UE_LOGF(LogHeartNet, Log, "Server: Client undoing action.")
	Proxy->ExecuteUndo_Client();
	UE_LOGF(LogHeartNet, Log, "Server: Client undid action.")
}

void UHeartNetClient::Server_RedoGraphAction_Implementation(UHeartGraphNetProxy* Proxy)
{
	ensure(IsValid(Proxy));
	UE_LOGF(LogHeartNet, Log, "Server: Client redoing action")
	Proxy->ExecuteRedo_Client();
	UE_LOGF(LogHeartNet, Log, "Server: Client redid action.")
}