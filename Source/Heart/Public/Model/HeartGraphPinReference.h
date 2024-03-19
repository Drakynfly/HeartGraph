// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.h"
#include "HeartGraphPinReference.generated.h"

USTRUCT(BlueprintType)
struct FHeartGraphPinReference
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GraphPinReference")
	FHeartNodeGuid NodeGuid;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GraphPinReference")
	FHeartPinGuid PinGuid;

	bool IsValid() const
	{
		return NodeGuid.IsValid() && PinGuid.IsValid();
	}

	friend bool operator==(const FHeartGraphPinReference& Lhs, const FHeartGraphPinReference& Rhs)
	{
		return Lhs.NodeGuid == Rhs.NodeGuid
			   && Lhs.PinGuid == Rhs.PinGuid;
	}

	friend bool operator!=(const FHeartGraphPinReference& Lhs, const FHeartGraphPinReference& Rhs)
	{
		return !(Lhs == Rhs);
	}
};

FORCEINLINE uint32 GetTypeHash(const FHeartGraphPinReference& PinReference)
{
	return HashCombine(GetTypeHash(PinReference.NodeGuid), GetTypeHash(PinReference.PinGuid));
}

USTRUCT(BlueprintType)
struct FHeartGraphPinConnections
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GraphPinConnections")
	TSet<FHeartGraphPinReference> Links;
};