// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraph.h"
#include "General/HeartFlakes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "HeartGraphNetProxy.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHeartNet, Log, All)

struct FHeartReplicatedGraphNodes;

USTRUCT()
struct FHeartReplicatedNodeData : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FHeartNodeGuid NodeGuid;

	UPROPERTY()
	FHeartFlake FlakeData;

	void PostReplicatedAdd(const FHeartReplicatedGraphNodes& Array);
	void PostReplicatedChange(const FHeartReplicatedGraphNodes& Array);
	void PreReplicatedRemove(const FHeartReplicatedGraphNodes& Array);

	void PostReplicatedReceive(const FFastArraySerializer::FPostReplicatedReceiveParameters& Parameters);

	FString GetDebugString();
};

USTRUCT()
struct FHeartReplicatedGraphNodes : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FHeartReplicatedNodeData> Items;

	int32 IndexOf(const FHeartNodeGuid& Guid) const;
	void Operate(const FHeartNodeGuid& Guid, const TFunctionRef<void(FHeartReplicatedNodeData&)>& Func);
	bool Delete(const FHeartNodeGuid& Guid);

	TWeakObjectPtr<class UHeartGraphNetProxy> OwningProxy;

	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) {}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FHeartReplicatedNodeData, FHeartReplicatedGraphNodes>(Items, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FHeartReplicatedGraphNodes> : public TStructOpsTypeTraitsBase2<FHeartReplicatedGraphNodes>
{
	enum
	{
		WithNetDeltaSerializer = true,
    };
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeartNetProxyNodeEvent, UHeartGraphNode*, Node);


/**
 * This class represents a Heart Graph over the network, and can replicate its data between connections.
 */
UCLASS(BlueprintType, Within = Actor)
class HEART_API UHeartGraphNetProxy : public UObject
{
	GENERATED_BODY()

	friend FHeartReplicatedNodeData;

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
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Heart|NetProxy")
	static UHeartGraphNetProxy* CreateHeartNetProxy(AActor* Owner, UHeartGraph* SourceGraph);

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

	virtual void OnNodeAdded_Source(UHeartGraphNode* HeartGraphNode);
	virtual void OnNodesMoved_Source(const FHeartNodeMoveEvent& NodeMoveEvent);
	virtual void OnNodeRemoved_Source(UHeartGraphNode* HeartGraphNode);
	virtual void OnNodeConnectionsChanged_Source(const FHeartGraphConnectionEvent& GraphConnectionEvent);

	virtual bool ShouldReplicateNode(UHeartGraphNode* Node) const;

	void UpdateReplicatedNodeData(UHeartGraphNode* Node);
	void RemoveReplicatedNodeData(const FHeartNodeGuid& Node);


	/**-------------------------*/
	/*		PROXY GRAPH			*/
	/**-------------------------*/

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|NetProxy")
	UHeartGraph* GetProxiedGraph() const;

protected:
	UFUNCTION()
	virtual void OnRep_GraphClass();

	virtual void OnNodeAdded_Proxy(UHeartGraphNode* HeartGraphNode);
	virtual void OnNodesMoved_Proxy(const FHeartNodeMoveEvent& NodeMoveEvent);
	virtual void OnNodeRemoved_Proxy(UHeartGraphNode* HeartGraphNode);
	virtual void OnNodeConnectionsChanged_Proxy(const FHeartGraphConnectionEvent& GraphConnectionEvent);

	bool UpdateNodeProxy(const FHeartReplicatedNodeData& NodeData);
	bool RemoveNodeProxy(const FHeartNodeGuid& Node);

public:
	UPROPERTY(BlueprintAssignable, Category = "Heart|NetProxy|Events")
	FHeartNetProxyNodeEvent OnNodeProxyUpdated;

protected:
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
};