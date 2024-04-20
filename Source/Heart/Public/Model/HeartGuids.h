// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.generated.h"

// Base type for Guids that identify types of Heart classes.
USTRUCT()
struct FHeartGuid : public FGuid
{
	GENERATED_BODY()

	FHeartGuid() {}

	// Explicit converter to a typed Heart Guid
	template <
		typename T
		UE_REQUIRES(TIsDerivedFrom<T, FHeartGuid>::Value)
	>
	FORCEINLINE T Get() const
	{
		return *reinterpret_cast<const T*>(this);
	}

protected:
	FHeartGuid(const FGuid Guid)
	  : FGuid(Guid) {}
};


// Identifier for Heart Graphs
USTRUCT()
struct HEART_API FHeartGraphGuid : public FHeartGuid
{
	GENERATED_BODY()

	using FHeartGuid::FHeartGuid;
private:
	FHeartGraphGuid(const FGuid Guid)
	  : FHeartGuid(Guid) {}

public:
	static FHeartGraphGuid New() { return NewGuid(); }
};


// Identifier for Heart Graph Extensions
USTRUCT(BlueprintType)
struct HEART_API FHeartExtensionGuid : public FHeartGuid
{
	GENERATED_BODY()

	using FHeartGuid::FHeartGuid;
private:
	FHeartExtensionGuid(const FGuid Guid)
	  : FHeartGuid(Guid) {}

public:
	static FHeartExtensionGuid New() { return NewGuid(); }
};


// Identifier for Heart Graph Nodes
USTRUCT(BlueprintType)
struct HEART_API FHeartNodeGuid : public FHeartGuid
{
	GENERATED_BODY()

	using FHeartGuid::FHeartGuid;
private:
	FHeartNodeGuid(const FGuid Guid)
	  : FHeartGuid(Guid) {}

public:
	static FHeartNodeGuid New() { return NewGuid(); }
};


// Identifier for Heart Graph Pins
USTRUCT(BlueprintType)
struct HEART_API FHeartPinGuid : public FHeartGuid
{
	GENERATED_BODY()

	using FHeartGuid::FHeartGuid;
private:
	FHeartPinGuid(const FGuid Guid)
	  : FHeartGuid(Guid) {}

public:
	static FHeartPinGuid New() { return NewGuid(); }
};