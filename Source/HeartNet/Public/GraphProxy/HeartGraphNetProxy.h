// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartReplicatedData.h"
#include "NativeGameplayTags.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphTypes.h"
#include "Model/HeartGuids.h"
#include "HeartGraphNetProxy.generated.h"

struct FHeartGraphConnectionEvent;
struct FHeartGraphConnectionEvent_Net;
struct FHeartManualEvent;
struct FHeartNodeMoveEvent;
struct FHeartNodeMoveEvent_Net;
struct FHeartRemoteGraphActionArguments;
class UHeartActionBase;
class UHeartGraph;
class UHeartGraphExtension;
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

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Extension_Added)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Extension_Removed)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(NodeComponent_Added)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(NodeComponent_Removed)

	// This tag can be used for custom replicating events, but it's suggested to create unique tags.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Other)

	// Permissions-only tag for allowing clients to run graph action edits.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Permission_Actions)

	// Permissions-only tag for allowing clients to undo and redo graph action edits.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Permission_UndoRedo)

	// Permissions-only tag for allowing any kind of client edit.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Permission_All)
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHeartNetProxyNodeEvent, UHeartGraphNode*, Node, FGameplayTag, EditType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHeartNetProxyExtensionEvent, UHeartGraphExtension*, Node, FGameplayTag, EditType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHeartNetProxyNodeComponentEvent, const FHeartNodeGuid&, Node, UHeartGraphNodeComponent*, Component, FGameplayTag, EditType);

/**
 * This class represents a Heart Graph over the network, and can replicate its data between connections.
 */
UCLASS(BlueprintType, Within = Actor)
class HEARTNET_API UHeartGraphNetProxy : public UObject
{
	GENERATED_BODY()

	friend FHeartReplicatedFlake;
	friend FHeartReplicatedNodeComponent;
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

	// These functions are callbacks for when the source graph is changed, which propagate via replication to clients.
	virtual void OnNodeAddedOrRemoved_Source(const FHeartNodeAddOrRemoveEvent& AddOrRemoveEvent);
	virtual void OnNodesMoved_Source(const FHeartNodeMoveEvent& NodeMoveEvent);
	virtual void OnNodeConnectionsChanged_Source(const FHeartGraphConnectionEvent& GraphConnectionEvent);
	virtual void OnExtensionAddedOrRemoved_Source(UHeartGraphExtension* Extension, Heart::Events::EComponentEventType Type);
	virtual void OnNodeComponentAddedOrRemoved_Source(const FHeartNodeGuid& Node, UHeartGraphNodeComponent* Component, Heart::Events::EComponentEventType Type);

	virtual bool ShouldReplicateNode(TObjectPtr<UHeartGraphNode> Node) const;
	virtual bool ShouldReplicateExtension(TObjectPtr<UHeartGraphExtension> Extension) const;
	virtual bool ShouldReplicateNodeComponent(const FHeartNodeGuid& Node, UHeartGraphNodeComponent* Component) const;

	void UpdateReplicatedNodeData(TObjectPtr<UHeartGraphNode> Node);
	void UpdateReplicatedExtensionData(TObjectPtr<UHeartGraphExtension> Extension);
	void UpdateReplicatedNodeComponentData(const FHeartNodeGuid& Node, const UHeartGraphNodeComponent* Component);

	void EditReplicatedNodeData(const FHeartReplicatedFlake& NodeData, FGameplayTag EventType);
	void EditReplicatedExtensionData(const FHeartReplicatedFlake& ExtensionData, FGameplayTag EventType);
	void EditReplicatedNodeComponentData(const FHeartReplicatedNodeComponent& ComponentData, FGameplayTag EventType);


	/**-------------------------*/
	/*		PROXY GRAPH			*/
	/**-------------------------*/

public:
	// Is this graph a net-proxy?
	UFUNCTION(BlueprintPure, Category = "Heart|NetProxy")
	static bool IsProxyGraph(const UHeartGraph* Graph);

	UFUNCTION(BlueprintCallable, Category = "Heart|NetProxy")
	UHeartGraph* GetProxiedGraph() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heart|NetProxy", meta = (Categories = "Heart.Net"))
	void SetPermissions(const FGameplayTagContainer& Permissions);

	// A generic update function to send local node state to the server.
	UFUNCTION(BlueprintCallable, Category = "Heart|NetProxy")
	void RequestUpdateNode_OnServer(const FHeartNodeGuid& NodeGuid, EHeartUpdateNodeType Type = EHeartUpdateNodeType::HeartNode);

	// Use this to RPC graph actions on the server, to make changes to the source graph.
	UFUNCTION(BlueprintCallable, Category = "Heart|NetProxy")
	void ExecuteGraphActionOnServer(TSubclassOf<UHeartActionBase> Action, UObject* Target, const FHeartManualEvent& Activation, UObject* ContextObject);

	// Use this to RPC an undo request to the server.
	UFUNCTION(BlueprintCallable, Category = "Heart|NetProxy")
	void ExecuteUndoOnServer();

	// Use this to RPC a redo request to the server.
	UFUNCTION(BlueprintCallable, Category = "Heart|NetProxy")
	void ExecuteRedoOnServer();

protected:
	UFUNCTION(/*  Replication UFunction  */)
	virtual void OnRep_GraphClass();

	UFUNCTION(/*  Replication UFunction  */)
	virtual void OnRep_ClientPermissions(const FGameplayTagContainer& OldPermissions);

	virtual bool CanClientPerformEvent(FGameplayTag RequestedEventType) const;

	// These functions are callbacks for when the proxy graph is editing locally on the client machine.
	virtual void OnNodeAddedOrRemoved_Proxy(const FHeartNodeAddOrRemoveEvent& AddOrRemoveEvent);
	virtual void OnNodesMoved_Proxy(const FHeartNodeMoveEvent& NodeMoveEvent);
	virtual void OnNodeConnectionsChanged_Proxy(const FHeartGraphConnectionEvent& GraphConnectionEvent);
	virtual void OnExtensionAddedOrRemoved_Proxy(UHeartGraphExtension* Extension, Heart::Events::EComponentEventType Type);
	virtual void OnNodeComponentAddedOrRemoved_Proxy(const FHeartNodeGuid& Node, UHeartGraphNodeComponent* NodeComponent, Heart::Events::EComponentEventType Type);

	// These functions are called on the server via RPC when a client wants to edit things.
	virtual void OnNodeAdded_Client(const FHeartReplicatedFlake& NodeData);
	virtual void OnNodeRemoved_Client(const FHeartNodeGuid& NodeGuid);
	virtual void OnNodesMoved_Client(const FHeartNodeMoveEvent_Net& NodeMoveEvent);
	virtual void OnNodeConnectionsChanged_Client(const FHeartGraphConnectionEvent_Net& GraphConnectionEvent);

	virtual void UpdateNodeData_Client(const FHeartReplicatedFlake& NodeData, FGameplayTag EventType);

	virtual void OnExtensionAdded_Client(const FHeartReplicatedFlake& ExtensionData);
	virtual void OnExtensionRemoved_Client(const FHeartExtensionGuid& Extension);

	virtual void OnNodeComponentAdded_Client(const FHeartReplicatedNodeComponent& ComponentData);
	virtual void OnNodeComponentRemoved_Client(const FHeartNodeGuid& Node, const FHeartExtensionGuid& Component);

	virtual void ExecuteGraphAction_Client(TSubclassOf<UHeartActionBase> Action, const FHeartRemoteGraphActionArguments& Args);
	virtual void ExecuteUndo_Client();
	virtual void ExecuteRedo_Client();

	void UpdateNodeProxy(const FHeartReplicatedFlake& Data, FGameplayTag EventType);
	void RemoveNodeProxy(const FHeartReplicatedFlake& Data);

	void UpdateExtensionProxy(const FHeartReplicatedFlake& Data, FGameplayTag EventType);
	void RemoveExtensionProxy(const FHeartReplicatedFlake& Data);

	void UpdateNodeComponentProxy(const FHeartReplicatedNodeComponent& Data, FGameplayTag EventType);
	void RemoveNodeComponentProxy(const FHeartReplicatedNodeComponent& Data);

	void PostReplicatedAdd(const FHeartReplicatedData& Array, const FHeartReplicatedFlake& Data);
	void PostReplicatedChange(const FHeartReplicatedData& Array, const FHeartReplicatedFlake& Data);
	void PreReplicatedRemove(const FHeartReplicatedData& Array, const FHeartReplicatedFlake& Data);

	void PostReplicatedAdd_NodeComponent(const FHeartReplicatedNodeComponent& Data);
	void PostReplicatedChange_NodeComponent(const FHeartReplicatedNodeComponent& Data);
	void PreReplicatedRemove_NodeComponent(const FHeartReplicatedNodeComponent& Data);


	/**-----------------------------*/
	/*		NET PROXY EVENTS		*/
	/**-----------------------------*/

public:
	// Called when clients edit source nodes via RPC.
	UPROPERTY(BlueprintAssignable, Category = "Heart|NetProxy|Events")
	FHeartNetProxyNodeEvent OnNodeSourceEdited;

	// Called when clients edit source extensions via RPC.
	UPROPERTY(BlueprintAssignable, Category = "Heart|NetProxy|Events")
	FHeartNetProxyExtensionEvent OnExtensionSourceEdited;

	// Called when clients edit source node components via RPC.
	UPROPERTY(BlueprintAssignable, Category = "Heart|NetProxy|Events")
	FHeartNetProxyNodeComponentEvent OnNodeComponentSourceEdited;

	// Called when server edits proxy nodes via replication.
	UPROPERTY(BlueprintAssignable, Category = "Heart|NetProxy|Events")
	FHeartNetProxyNodeEvent OnNodeProxyUpdated;

	// Called when server edits proxy extensions via replication.
	UPROPERTY(BlueprintAssignable, Category = "Heart|NetProxy|Events")
	FHeartNetProxyExtensionEvent OnExtensionProxyUpdated;

	// Called when server edits proxy extensions via replication.
	UPROPERTY(BlueprintAssignable, Category = "Heart|NetProxy|Events")
	FHeartNetProxyNodeComponentEvent OnNodeComponentProxyUpdated;


	/**-------------------------*/
	/*		PROXY CONFIG		*/
	/**-------------------------*/

protected:
	// Container to set up the types of edits a client connection can make to the source graph. To allow all client edits,
	// add the "Heart.Net.AllPermissions" tag. Replicated to let client know what actions they can perform.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = "SetPermissions", ReplicatedUsing = "OnRep_ClientPermissions", Category = "Config", meta = (Categories = "Heart.Net"))
	FGameplayTagContainer ClientPermissions;

	// Client component used to RPC edits. Only ever valid on the client machine.
	UPROPERTY(BlueprintReadOnly, Category = "Config")
	TObjectPtr<UHeartNetClient> LocalClient;

	// Should the client try to log actions that it RPCs locally?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool LogActionsClientside;


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

	// Replication of UHeartGraph::Nodes from SourceGraph to ProxyGraph
	UPROPERTY(Replicated)
	FHeartReplicatedData ReplicatedNodes;

	// Replication of UHeartGraph::Extensions from SourceGraph to ProxyGraph
	UPROPERTY(Replicated)
	FHeartReplicatedData ReplicatedExtensions;

	// Replication of UHeartGraph::NodeComponents from SourceGraph to ProxyGraph
	UPROPERTY(Replicated)
	FHeartReplicatedNodeComponents ReplicatedNodeComponents;

private:
	enum ERecursiveCheck
	{
		NodeAdd,
		NodeDelete,
		ExtAdd,
		ExtDelete,
		CompAdd,
		CompDelete,
		MAX
	};
	bool RecursionGuards[ERecursiveCheck::MAX] = {};
};