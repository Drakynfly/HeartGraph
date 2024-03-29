﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartReplicateNodeData.h"
#include "NativeGameplayTags.h"
#include "Model/HeartGuids.h"
#include "HeartGraphNetProxy.generated.h"

struct FHeartGraphConnectionEvent;
struct FHeartGraphConnectionEvent_Net;
struct FHeartManualEvent;
struct FHeartNodeMoveEvent;
struct FHeartNodeMoveEvent_Net;
struct FHeartRemoteGraphActionArguments;
class UHeartGraph;
class UHeartGraphActionBase;
class UHeartGraphNode;
class UHeartNetClient;

UENUM()
enum class EHeartUpdateNodeType : uint8
{
	None UMETA(hidden),
	HeartNode,
	NodeObject
};

namespace Heart::Net::Tags
{
	// Tags used to identify types of replicated changes.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Node_Added)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Node_Removed)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Node_Moved)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Node_ConnectionsChanged)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Node_ClientUpdateNodeObject)

	// This tag can be used for custom replicating events, but its suggested to create unique tags.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Other)

	// Permissions-only tag for allowing clients to run graph action edits.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Permission_Actions)

	// Permissions-only tag for allowing any kind of client edit.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Permission_All)
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHeartNetProxyNodeEvent, UHeartGraphNode*, Node, FGameplayTag, EditType);

/**
 * This class represents a Heart Graph over the network, and can replicate its data between connections.
 */
UCLASS(BlueprintType, Within = Actor)
class HEARTNET_API UHeartGraphNetProxy : public UObject
{
	GENERATED_BODY()

	friend FHeartReplicatedNodeData;
	friend UHeartNetClient;

public:
	UHeartGraphNetProxy();

	//~ UObject
	virtual UWorld* GetWorld() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;
	//~ UObject

	UFUNCTION(BlueprintCallable, Category = "Heart|NetProxy")
	AActor* GetOwningActor() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|NetProxy")
	UHeartGraph* GetGraph() const;


	/**-------------------------*/
	/*		NET LIFETIME		*/
	/**-------------------------*/
public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heart|NetProxy", meta = (AdvancedDisplay = 2))
	static UHeartGraphNetProxy* CreateHeartNetProxy(AActor* Owner, UHeartGraph* SourceGraph, TSubclassOf<UHeartGraphNetProxy> ProxyClassOverride);

	// Set the net client used to replicate local changes to this graph to the server.
	UFUNCTION(BlueprintCallable, Category = "Heart|NetProxy")
	void SetLocalClient(UHeartNetClient* NetClient);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heart|NetProxy")
	void Destroy();

protected:
	virtual void OnDestroyed() {}


	/**-------------------------*/
	/*		SOURCE GRAPH		*/
	/**-------------------------*/

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heart|NetProxy")
	void RequestUpdateNode(UHeartGraphNode* Node);

protected:
	bool SetupGraphProxy(UHeartGraph* InSourceGraph);

	// These functions are callbacks for when the source graph is changed, which propogate via replication to clients.
	virtual void OnNodeAdded_Source(UHeartGraphNode* HeartGraphNode);
	virtual void OnNodesMoved_Source(const FHeartNodeMoveEvent& NodeMoveEvent);
	virtual void OnNodeRemoved_Source(UHeartGraphNode* HeartGraphNode);
	virtual void OnNodeConnectionsChanged_Source(const FHeartGraphConnectionEvent& GraphConnectionEvent);

	virtual bool ShouldReplicateNode(TObjectPtr<UHeartGraphNode> Node) const;

	void UpdateReplicatedNodeData(TObjectPtr<UHeartGraphNode> Node);
	void RemoveReplicatedNodeData(const FHeartNodeGuid& Node);
	void EditReplicatedNodeData(const FHeartNodeFlake& NodeData, FGameplayTag EventType);


	/**-------------------------*/
	/*		PROXY GRAPH			*/
	/**-------------------------*/

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|NetProxy")
	UHeartGraph* GetProxiedGraph() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heart|NetProxy", meta = (Categories = "Heart.Net"))
	void SetPermissions(const FGameplayTagContainer& Permissions);

	// A generic update function to send local node state to the server.
	UFUNCTION(BlueprintCallable, Category = "Heart|NetProxy")
	void RequestUpdateNode_OnServer(FHeartNodeGuid NodeGuid, EHeartUpdateNodeType Type = EHeartUpdateNodeType::HeartNode);

	// Use this to remote execute graph actions on the server, to make changes to the source graph.
	UFUNCTION(BlueprintCallable, Category = "Heart|NetProxy")
	void ExecuteGraphActionOnServer(UHeartGraphActionBase* Action, UObject* Target, const FHeartManualEvent& Activation, UObject* ContextObject);

protected:
	UFUNCTION()
	virtual void OnRep_GraphClass();

	UFUNCTION()
	virtual void OnRep_ClientPermissions(const FGameplayTagContainer& OldPermissions);

	virtual bool CanClientPerformEvent(FGameplayTag RequestedEventType) const;

	// These functions are callbacks for when the proxy graph is editing locally on the client machine.
	virtual void OnNodeAdded_Proxy(UHeartGraphNode* HeartGraphNode);
	virtual void OnNodesMoved_Proxy(const FHeartNodeMoveEvent& NodeMoveEvent);
	virtual void OnNodeRemoved_Proxy(UHeartGraphNode* HeartGraphNode);
	virtual void OnNodeConnectionsChanged_Proxy(const FHeartGraphConnectionEvent& GraphConnectionEvent);

	// These functions are called on the server via RPC when a client wants to edit things.
	virtual void OnNodeAdded_Client(const FHeartNodeFlake& NodeData);
	virtual void OnNodesMoved_Client(const FHeartNodeMoveEvent_Net& NodeMoveEvent);
	virtual void OnNodeRemoved_Client(FHeartNodeGuid NodeGuid);
	virtual void OnNodeConnectionsChanged_Client(const FHeartGraphConnectionEvent_Net& GraphConnectionEvent);

	virtual void UpdateNodeData_Client(const FHeartNodeFlake& NodeData, FGameplayTag EventType);
	virtual void ExecuteGraphAction_Client(const FHeartFlake& ActionData, const FHeartRemoteGraphActionArguments& Args);

	bool UpdateNodeProxy(const FHeartReplicatedNodeData& NodeData, FGameplayTag EventType);
	bool RemoveNodeProxy(const FHeartNodeGuid& NodeGuid);


	/**-----------------------------*/
	/*		NET PROXY EVENTS		*/
	/**-----------------------------*/

public:
	// Called when clients edit source nodes via RPC.
	UPROPERTY(BlueprintAssignable, Category = "Heart|NetProxy|Events")
	FHeartNetProxyNodeEvent OnNodeSourceEdited;

	// Called when server edits proxy nodes via replication.
	UPROPERTY(BlueprintAssignable, Category = "Heart|NetProxy|Events")
	FHeartNetProxyNodeEvent OnNodeProxyUpdated;


	/**-------------------------*/
	/*		PROXY CONFIG		*/
	/**-------------------------*/

protected:
	// Container to setup the types of edits a client connection can make to the source graph. To allow all client edits
	// simply add the "Heart.Net.AllPermissions" tag. Replicated to let client know what actions they can perform.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = "SetPermissions", ReplicatedUsing = "OnRep_ClientPermissions", Category = "Config", meta = (Categories = "Heart.Net"))
	FGameplayTagContainer ClientPermissions;

	// Client component used to RPC edits. Only ever valid on the client machine.
	UPROPERTY(BlueprintReadOnly, Category = "Config")
	TObjectPtr<UHeartNetClient> LocalClient;


	/**-------------------------*/
	/*		INTERNAL STATE		*/
	/**-------------------------*/

	// Original graph pointer. Only valid from the server.
	UPROPERTY()
	TObjectPtr<UHeartGraph> SourceGraph;

	// Simulated graph proxy. Only valid from clients.
	UPROPERTY()
	TObjectPtr<UHeartGraph> ProxyGraph;

	UPROPERTY(ReplicatedUsing = "OnRep_GraphClass")
	TSubclassOf<UHeartGraph> GraphClass;

	UPROPERTY(Replicated)
	FHeartReplicatedGraphNodes ReplicatedNodes;

	bool RecursionGuard = false;
};