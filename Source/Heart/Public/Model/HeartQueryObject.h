// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartQueryObject.generated.h"

namespace Heart::Query
{
	class IMapQuery;
}

/**
 * A UObject wrapper around Heart Map Queries
 */
UCLASS()
class HEART_API UHeartQueryObject : public UObject
{
	GENERATED_BODY()
};