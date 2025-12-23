// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartReplicatedData.h"
#include "Components/ActorComponent.h"
#include "Model/HeartGraphExtension.h"
#include "Model/HeartGraphNodeComponent.h"
#include "Model/HeartGuids.h"
#include "HeartNetClient.generated.h"

enum class EHeartUpdateNodeType : uint8;
struct FFlake;
struct FHeartGraphConnectionEvent;
struct FHeartGraphConnectionEvent_Net;
struct FHeartNodeMoveEvent;
struct FHeartNodeMoveEvent_Net;
struct FHeartRemoteGraphActionArguments;
struct FHeartReplicatedFlake;
class UHeartActionBase;
class UHeartGraphNetProxy;
class UHeartGraphNode;


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
	UFUNCTION(Server, Reliable)
	void Server_OnNodeAdded(UHeartGraphNetProxy* Proxy, const FHeartReplicatedFlake& NodeData);

	UFUNCTION(Server, Reliable)
	void Server_OnNodesMoved(UHeartGraphNetProxy* Proxy, const FHeartNodeMoveEvent_Net& NodeMoveEvent);

	UFUNCTION(Server, Reliable)
	void Server_OnNodeRemoved(UHeartGraphNetProxy* Proxy, const FHeartNodeGuid& Node);

	UFUNCTION(Server, Reliable)
	void Server_OnNodeConnectionsChanged(UHeartGraphNetProxy* Proxy, const FHeartGraphConnectionEvent_Net& GraphConnectionEvent);

	UFUNCTION(Server, Reliable)
	void Server_OnExtensionAdded(UHeartGraphNetProxy* Proxy, const FHeartReplicatedFlake& ExtensionData);

	UFUNCTION(Server, Reliable)
	void Server_OnExtensionRemoved(UHeartGraphNetProxy* Proxy, const FHeartExtensionGuid& Extension);

	UFUNCTION(Server, Reliable)
	void Server_OnNodeComponentAdded(UHeartGraphNetProxy* Proxy, const FHeartReplicatedNodeComponent& ComponentData);

	UFUNCTION(Server, Reliable)
	void Server_OnNodeComponentRemoved(UHeartGraphNetProxy* Proxy, const FHeartNodeGuid& Node, const FHeartExtensionGuid& Component);

	UFUNCTION(Server, Reliable)
	void Server_UpdateGraphNode(UHeartGraphNetProxy* Proxy, const FHeartReplicatedFlake& NodeFlake, EHeartUpdateNodeType Type);

	UFUNCTION(Server, Reliable)
	void Server_ExecuteGraphAction(UHeartGraphNetProxy* Proxy, TSubclassOf<UHeartActionBase> Action, const FHeartRemoteGraphActionArguments& Args);

	UFUNCTION(Server, Reliable)
	void Server_UndoGraphAction(UHeartGraphNetProxy* Proxy);

	UFUNCTION(Server, Reliable)
	void Server_RedoGraphAction(UHeartGraphNetProxy* Proxy);
};