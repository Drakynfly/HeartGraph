// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "General/HeartFlakes.h"
#include "Model/HeartGuids.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "HeartReplicateNodeData.generated.h"

struct FHeartReplicatedGraphNodes;

USTRUCT()
struct FHeartNodeFlake
{
	GENERATED_BODY()

	// Node guid.
	UPROPERTY()
	FHeartNodeGuid Guid;

	// Data for this node. Can be anything; up to code path to interpret correctly
	UPROPERTY()
	FHeartFlake Flake;
};

USTRUCT()
struct FHeartReplicatedNodeData : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FHeartNodeFlake Data;

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