// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartGuids.generated.h"

USTRUCT()
struct FHeartGuid : public FGuid
{
	GENERATED_BODY()

	using FGuid::FGuid;
	FHeartGuid(const FGuid& Guid)
	  : FGuid(Guid) {}
};

USTRUCT()
struct FHeartGraphGuid : public FHeartGuid
{
	GENERATED_BODY()
	using FHeartGuid::FHeartGuid;
};

USTRUCT(BlueprintType)
struct FHeartNodeGuid : public FHeartGuid
{
	GENERATED_BODY()
	using FHeartGuid::FHeartGuid;
};

USTRUCT(BlueprintType)
struct FHeartPinGuid : public FHeartGuid
{
	GENERATED_BODY()
	using FHeartGuid::FHeartGuid;
};