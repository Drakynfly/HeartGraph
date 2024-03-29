﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "HeartPinDirection.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EHeartPinDirection : uint8
{
	None = 0 UMETA(Hidden),
	Input = 1 << 0,
	Output = 1 << 1,

	Bidirectional = Input | Output
};

ENUM_CLASS_FLAGS(EHeartPinDirection)
