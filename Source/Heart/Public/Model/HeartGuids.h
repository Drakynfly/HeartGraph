// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.generated.h"

USTRUCT()
struct FHeartGuid : public FGuid
{
	GENERATED_BODY()

	FHeartGuid() {}
protected:
	FHeartGuid(const FGuid Guid)
	  : FGuid(Guid) {}
};

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