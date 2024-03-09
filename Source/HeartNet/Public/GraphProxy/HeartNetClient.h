// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Model/HeartGuids.h"
#include "HeartNetClient.generated.h"

struct FHeartGraphConnectionEvent_Net;
struct FHeartNodeMoveEvent_Net;
struct FHeartGraphConnectionEvent;
struct FHeartNodeMoveEvent;
struct FHeartReplicatedNodeData;
class UHeartGraphNode;
class UHeartGraphNetProxy;


/**
 * An actor component that must be added to PlayerControllers to allow clients to modify Heart Graphs on the server.
 *
 * This component can handle replicating edits for multiple proxies. Each proxy must be added by calling SetLocalClient
 * on the Proxy.
 */
UCLASS(ClassGroup = ("Heart"), meta = (BlueprintSpawnableComponent))
class HEARTNET_API UHeartNetClient : public UActorComponent
{
	GENERATED_BODY()

	friend class UHeartGraphNetProxy;

public:
	UHeartNetClient();

protected:
	virtual void OnNodeAdded(UHeartGraphNetProxy* Proxy, UHeartGraphNode* HeartGraphNode);
	virtual void OnNodesMoved(UHeartGraphNetProxy* Proxy, const FHeartNodeMoveEvent& NodeMoveEvent);
	virtual void OnNodeRemoved(UHeartGraphNetProxy* Proxy, UHeartGraphNode* HeartGraphNode);
	virtual void OnNodeConnectionsChanged(UHeartGraphNetProxy* Proxy, const FHeartGraphConnectionEvent& GraphConnectionEvent);

	UFUNCTION(Server, Reliable)
	void Server_OnNodeAdded(UHeartGraphNetProxy* Proxy, const FHeartReplicatedNodeData& NodeData);

	UFUNCTION(Server, Reliable)
	void Server_OnNodesMoved(UHeartGraphNetProxy* Proxy, const FHeartNodeMoveEvent_Net& NodeMoveEvent);

	UFUNCTION(Server, Reliable)
	void Server_OnNodeRemoved(UHeartGraphNetProxy* Proxy, FHeartNodeGuid HeartGraphNode);

	UFUNCTION(Server, Reliable)
	void Server_OnNodeConnectionsChanged(UHeartGraphNetProxy* Proxy, const FHeartGraphConnectionEvent_Net& GraphConnectionEvent);
};