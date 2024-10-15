// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGraphPinDesc.h"
#include "HeartGraphPinReference.h"
#include "HeartGuids.h"

#include "Containers/Map.h"
#include "Misc/Optional.h"
#include "StructUtils/StructView.h"

#include "HeartPinData.generated.h"

namespace Heart
{
	namespace Query
	{
		class FPinQueryResult;
	}

	namespace Connections
	{
		class FEdit;
	}
}

// @todo this should not be BlueprintType. it only is temporarily until there is a way to view pins in the editor window without making PinData VisibleInstanceOnly
// Long-term, it would be nice to make this type not a USTRUCT at all, and swap out the TMaps for the faster one from VoxelCore
// This struct is *intentionally* not exported, as it should not be accessible to anything but UHeartGraphNode

/**
 * Container for all pin data on a Heart Node, including links to other nodes.
 */
USTRUCT(BlueprintType)
struct FHeartNodePinData
{
	GENERATED_BODY()

	friend class UHeartGraphNode;
	friend Heart::Query::FPinQueryResult;
	friend Heart::Connections::FEdit;

protected:
	void AddPin(FHeartPinGuid NewKey, const FHeartGraphPinDesc& Desc);
	bool RemovePin(FHeartPinGuid Key);

	bool Contains(FHeartPinGuid Key) const;
	int32 GetPinIndex(FHeartPinGuid Key) const;
	bool HasConnections(FHeartPinGuid Key) const;

	TOptional<FHeartGraphPinDesc> GetPinDesc(FHeartPinGuid Key) const;
	FHeartGraphPinDesc& GetPinDesc(FHeartPinGuid Key);

	TConstStructView<FHeartGraphPinConnections> ViewConnections(FHeartPinGuid Key) const;
	FHeartGraphPinConnections& GetConnectionsMutable(FHeartPinGuid Key);

	void AddConnection(const FHeartPinGuid Key, const FHeartGraphPinReference& Pin);
	bool RemoveConnection(const FHeartPinGuid Key, const FHeartGraphPinReference& Pin);

	FORCEINLINE const FHeartGraphPinConnections& operator[](const FHeartPinGuid Key)
	{
		return PinConnections.FindChecked(Key);
	}

	/**
	 * Try to find a pin by a predicate. Quicker than using a Query, but cannot be chained.
	 */
	template <typename Predicate>
	TOptional<FHeartPinGuid> Find(Predicate Pred) const
	{
		for (auto&& Element : PinDescriptions)
		{
			if (auto Result = Pred(Element);
				Result.IsSet())
			{
				return Result.GetValue();
			}
		}
		return NullOpt;
	}

	// Maps pins to their Pin Description, which carries all unique instance data about
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<FHeartPinGuid, FHeartGraphPinDesc> PinDescriptions;

	// Maps pins to their connections in other nodes.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<FHeartPinGuid, FHeartGraphPinConnections> PinConnections;

	// Maintains the original order of pins as added to the node.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<FHeartPinGuid, int32> PinOrder;
};