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

	friend FArchive& operator<<(FArchive& Ar, FHeartGraphPinReference& V)
	{
		return Ar << V.NodeGuid << V.PinGuid;
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

	friend struct FHeartNodePinData;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GraphPinConnections")
	TArray<FHeartGraphPinReference> Connections;

	// DEPRECATED
	UPROPERTY()
	TSet<FHeartGraphPinReference> Links;

public:
	TConstArrayView<FHeartGraphPinReference> GetLinks() const { return Connections; }

	bool Serialize(FArchive& Ar)
	{
		Ar << *this;
		return true;
	}

	void PostSerialize(const FArchive& Ar)
	{
		if (Ar.IsLoading())
		{
			if (!Links.IsEmpty())
			{
				Connections.Append(Links.Array());
				Links.Empty();
			}
		}
	}

	friend FArchive& operator<<(FArchive& Ar, FHeartGraphPinConnections& V)
	{
		Ar << V.Connections;
		return Ar;
	}

	using RangedForConstIteratorType = TArray<FHeartGraphPinReference>::RangedForConstIteratorType;

	FORCEINLINE RangedForConstIteratorType begin() const { return Connections.begin(); }
	FORCEINLINE RangedForConstIteratorType end()   const { return Connections.end(); }
};

template<>
struct TStructOpsTypeTraits<FHeartGraphPinConnections> : public TStructOpsTypeTraitsBase2<FHeartGraphPinConnections>
{
	enum
	{
		WithSerialize = true,
		WithPostSerialize = true,
	};
};