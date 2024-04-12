// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "GraphProxy/HeartNetClient.h"
#include "LogHeartNet.h"
#include "GraphProxy/HeartGraphNetProxy.h"
#include "GraphProxy/HeartNetReplicationTypes.h"
#include "Input/HeartActionBase.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNode3D.h"
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
	ensure(GetOwnerRole() == ROLE_AutonomousProxy);

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
	ensure(GetOwnerRole() == ROLE_AutonomousProxy);

	if (NodeMoveEvent.MoveFinished)
	{
		FHeartNodeMoveEvent_Net Event;
		Algo::Transform(NodeMoveEvent.AffectedNodes, Event.AffectedNodes,
			[](const TObjectPtr<UHeartGraphNode> Node)
			{
				FHeartNodeFlake NodeData;
				NodeData.Guid = Node->GetGuid();
				if (auto&& Node3D = Cast<UHeartGraphNode3D>(Node))
				{
					NodeData.Flake = Heart::Flakes::CreateFlake<FVector>(Node3D->GetLocation3D());
				}
				else
				{
					NodeData.Flake = Heart::Flakes::CreateFlake<FVector2D>(Node->GetLocation());
				}

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
	ensure(GetOwnerRole() == ROLE_AutonomousProxy);

	Server_OnNodeRemoved(Proxy, HeartGraphNode->GetGuid());
}

void UHeartNetClient::OnNodeConnectionsChanged(UHeartGraphNetProxy* Proxy,
	const FHeartGraphConnectionEvent& GraphConnectionEvent)
{
	check(Proxy);
	ensure(GetOwnerRole() == ROLE_AutonomousProxy);

	auto ByAffected = [&GraphConnectionEvent](const FHeartPinGuid Pin)
		{
			return GraphConnectionEvent.AffectedPins.Contains(Pin);
		};

	FHeartGraphConnectionEvent_Net Event;
	Algo::Transform(GraphConnectionEvent.AffectedNodes, Event.AffectedNodes,
		[&ByAffected](const TObjectPtr<UHeartGraphNode> Node)
		{
			FHeartNodeFlake NodeData;
			NodeData.Guid = Node->GetGuid();

			FHeartGraphConnectionEvent_Net_PinElement PinElement;
			Node->QueryPins()
				.Filter(ByAffected)
				.ForEach([Node, &PinElement](const FHeartPinGuid Pin)
				{
					auto Connections = Node->GetConnections(Pin);
					PinElement.PinConnections.Add(Pin, Connections.GetValue());
				});

			NodeData.Flake = Heart::Flakes::CreateFlake(PinElement);

			UE_LOG(LogHeartNet, Log, TEXT("Sending node RPC data '%s' (%i bytes)"),
				*Node->GetName(), NodeData.Flake.Data.Num());

			return NodeData;
		});

	Server_OnNodeConnectionsChanged(Proxy, Event);
}

void UHeartNetClient::Server_UndoGraphAction_Implementation(UHeartGraphNetProxy* Proxy)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client undoing action."))
	Proxy->ExecuteUndo_Client();
}

void UHeartNetClient::Server_RedoGraphAction_Implementation(UHeartGraphNetProxy* Proxy)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client redoing action"))
	Proxy->ExecuteRedo_Client();
}

void UHeartNetClient::Server_UpdateGraphNode_Implementation(UHeartGraphNetProxy* Proxy,
															const FHeartNodeFlake& NodeFlake, const EHeartUpdateNodeType Type)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client updated node. (%i bytes) Type '%s'"), NodeFlake.Flake.Data.Num(), *StaticEnum<EHeartUpdateNodeType>()->GetValueAsString(Type))
	Proxy->UpdateNodeData_Client(NodeFlake, Type == EHeartUpdateNodeType::NodeObject ? Heart::Net::Tags::Node_ClientUpdateNodeObject : Heart::Net::Tags::Other);
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

void UHeartNetClient::Server_ExecuteGraphAction_Implementation(UHeartGraphNetProxy* Proxy, const TSubclassOf<UHeartActionBase> Action,
															   const FHeartRemoteGraphActionArguments& Args)
{
	ensure(IsValid(Proxy));
	UE_LOG(LogHeartNet, Log, TEXT("Server: Client wants to run graph action '%s'"), *Action->GetName())
	Proxy->ExecuteGraphAction_Client(Action, Args);
}