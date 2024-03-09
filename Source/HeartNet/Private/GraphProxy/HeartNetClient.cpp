// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphProxy/HeartNetClient.h"
#include "LogHeartNet.h"
#include "GraphProxy/HeartGraphNetProxy.h"
#include "GraphProxy/HeartNetReplicationTypes.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartNetClient)

UHeartNetClient::UHeartNetClient()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UHeartNetClient::OnNodeAdded(UHeartGraphNetProxy* Proxy, UHeartGraphNode* HeartGraphNode)
{
	check(Proxy);

	FHeartReplicatedNodeData NodeData;
	NodeData.Data.Guid = HeartGraphNode->GetGuid();
	NodeData.Data.Flake = Heart::Flakes::CreateFlake(HeartGraphNode);
	UE_LOG(LogHeartNet, Log, TEXT("Sending node RPC data '%s' (%i bytes)"),
		*HeartGraphNode->GetName(), NodeData.Data.Flake.Data.Num());

	Server_OnNodeAdded(Proxy, NodeData);
}

void UHeartNetClient::OnNodesMoved(UHeartGraphNetProxy* Proxy, const FHeartNodeMoveEvent& NodeMoveEvent)
{
	check(Proxy);

	if (NodeMoveEvent.MoveFinished)
	{
		FHeartNodeMoveEvent_Net Event;
		Algo::Transform(NodeMoveEvent.AffectedNodes, Event.AffectedNodes,
			[](const TObjectPtr<UHeartGraphNode> Node)
			{
				FHeartNodeFlake NodeData;
				NodeData.Guid = Node->GetGuid();
				NodeData.Flake = Heart::Flakes::CreateFlake(Node);
				UE_LOG(LogHeartNet, Log, TEXT("Sending node RPC data '%s' (%i bytes)"),
					*Node->GetName(), NodeData.Flake.Data.Num());
				return NodeData;
			});

		Event.MoveFinished = NodeMoveEvent.MoveFinished;

		Server_OnNodesMoved(Proxy, Event);
	}
}

void UHeartNetClient::OnNodeRemoved(UHeartGraphNetProxy* Proxy, UHeartGraphNode* HeartGraphNode)
{
	check(Proxy);

	Server_OnNodeRemoved(Proxy, HeartGraphNode->GetGuid());
}

void UHeartNetClient::OnNodeConnectionsChanged(UHeartGraphNetProxy* Proxy,
	const FHeartGraphConnectionEvent& GraphConnectionEvent)
{
	check(Proxy);

	FHeartGraphConnectionEvent_Net Event;
	Algo::Transform(GraphConnectionEvent.AffectedNodes, Event.AffectedNodes,
		[](const TObjectPtr<UHeartGraphNode> Node)
		{
			FHeartNodeFlake NodeData;
			NodeData.Guid = Node->GetGuid();
			NodeData.Flake = Heart::Flakes::CreateFlake(Node);
			UE_LOG(LogHeartNet, Log, TEXT("Sending node RPC data '%s' (%i bytes)"),
				*Node->GetName(), NodeData.Flake.Data.Num());
			return NodeData;
		});

	Server_OnNodeConnectionsChanged(Proxy, Event);
}

void UHeartNetClient::Server_OnNodeAdded_Implementation(UHeartGraphNetProxy* Proxy, const FHeartReplicatedNodeData& HeartGraphNode)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client added node. (%i bytes)"), HeartGraphNode.Data.Flake.Data.Num())
	Proxy->OnNodeAdded_Client(HeartGraphNode.Data);
}

void UHeartNetClient::Server_OnNodesMoved_Implementation(UHeartGraphNetProxy* Proxy,
														 const FHeartNodeMoveEvent_Net& NodeMoveEvent)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client moved node"))
	Proxy->OnNodesMoved_Client(NodeMoveEvent);
}

void UHeartNetClient::Server_OnNodeRemoved_Implementation(UHeartGraphNetProxy* Proxy, const FHeartNodeGuid HeartGraphNode)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client removed node"))
	Proxy->OnNodeRemoved_Client(HeartGraphNode);
}

void UHeartNetClient::Server_OnNodeConnectionsChanged_Implementation(UHeartGraphNetProxy* Proxy,
																	 const FHeartGraphConnectionEvent_Net& GraphConnectionEvent)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client changed node connections"))
	Proxy->OnNodeConnectionsChanged_Client(GraphConnectionEvent);
}