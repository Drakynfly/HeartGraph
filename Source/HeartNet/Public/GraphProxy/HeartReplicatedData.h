// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "FlakesInterface.h"
#include "Model/HeartGuids.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "HeartReplicatedData.generated.h"

struct FHeartReplicatedData;

/*
 * A replicated flake, used as a FastArray Item.
 */
USTRUCT()
struct FHeartReplicatedFlake : public FFastArraySerializerItem
{
	GENERATED_BODY()

	// Heart Guid; can be a FHeartExtensionGuid or FHeartNodeGuid
	UPROPERTY()
	FHeartGuid Guid;

	// Data for this extension. Can be anything; up to code-path to interpret correctly
	UPROPERTY()
	FFlake Flake;

	void PostReplicatedAdd(const FHeartReplicatedData& Array);
	void PostReplicatedChange(const FHeartReplicatedData& Array);
	void PreReplicatedRemove(const FHeartReplicatedData& Array);

	void PostReplicatedReceive(const FFastArraySerializer::FPostReplicatedReceiveParameters& Parameters);

	FString GetDebugString();
};

/*
 * A fast array of Replicated Flakes.
 */
USTRUCT()
struct FHeartReplicatedData : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FHeartReplicatedFlake> Items;

	int32 IndexOf(const FHeartGuid& Guid) const;
	void Operate(const FHeartGuid& Guid, const TFunctionRef<void(FHeartReplicatedFlake&)>& Func);
	bool Delete(const FHeartGuid& Guid);

	TWeakObjectPtr<class UHeartGraphNetProxy> OwningProxy;

	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) {}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FHeartReplicatedFlake, FHeartReplicatedData>(Items, DeltaParms, *this);
	}
};

struct FHeartReplicatedNodeComponents;

/*
 * A replicated flake, used as a FastArray Item.
 */
USTRUCT()
struct FHeartReplicatedNodeComponent : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FHeartNodeGuid NodeGuid;

	UPROPERTY()
	FHeartExtensionGuid ComponentGuid;

	// Data for this component. Can be anything; up to code-path to interpret correctly
	UPROPERTY()
	FFlake Flake;

	void PostReplicatedAdd(const FHeartReplicatedNodeComponents& Array);
	void PostReplicatedChange(const FHeartReplicatedNodeComponents& Array);
	void PreReplicatedRemove(const FHeartReplicatedNodeComponents& Array);

	void PostReplicatedReceive(const FFastArraySerializer::FPostReplicatedReceiveParameters& Parameters);

	FString GetDebugString();
};

/*
 * A fast array of Replicated Flakes.
 */
USTRUCT()
struct FHeartReplicatedNodeComponents : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FHeartReplicatedNodeComponent> Items;

	int32 IndexOf(const FHeartExtensionGuid& Guid) const;
	void Operate(const FHeartExtensionGuid& Guid, const TFunctionRef<void(FHeartReplicatedNodeComponent&)>& Func);
	bool Delete(const FHeartExtensionGuid& Guid);

	TWeakObjectPtr<class UHeartGraphNetProxy> OwningProxy;

	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) {}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FHeartReplicatedNodeComponent, FHeartReplicatedNodeComponents>(Items, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FHeartReplicatedData> : public TStructOpsTypeTraitsBase2<FHeartReplicatedData>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};