// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InputCoreTypes.h"

enum EHeartTripType
{
	Unknown,
	Press,
	Release,
	Manual
};

struct FHeartWidgetInputTrip
{
	EHeartTripType Type = Unknown;
	FKey Key = EKeys::Invalid;
	FName CustomKey = NAME_None;

	bool IsValid() const
	{
		return Type != Unknown && (Key.IsValid() || !CustomKey.IsNone());
	}

	friend bool operator==(const FHeartWidgetInputTrip& Lhs, const FHeartWidgetInputTrip& Rhs)
	{
		return Lhs.Type == Rhs.Type &&
			   Lhs.Key == Rhs.Key &&
			   Lhs.CustomKey == Rhs.CustomKey;
	}

	friend bool operator!=(const FHeartWidgetInputTrip& Lhs, const FHeartWidgetInputTrip& Rhs)
	{
		return !(Lhs == Rhs);
	}
};

FORCEINLINE uint32 GetTypeHash(const FHeartWidgetInputTrip& Trip)
{
	uint32 KeyHash = 0;
	KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.Type));
	KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.Key));
	KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.CustomKey));
	return KeyHash;
}