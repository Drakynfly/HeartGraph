// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InputCoreTypes.h"

namespace Heart::Input
{
	enum ETripType
	{
		Unknown,
		Press,
		Release,
		Manual
	};

	struct FInputTrip
	{
		ETripType Type = Unknown;
		FKey Key = EKeys::Invalid;
		FName CustomKey = NAME_None;

		bool IsValid() const
		{
			return Type != Unknown && (Key.IsValid() || !CustomKey.IsNone());
		}

		friend bool operator==(const FInputTrip& Lhs, const FInputTrip& Rhs)
		{
			return Lhs.Type == Rhs.Type &&
				   Lhs.Key == Rhs.Key &&
				   Lhs.CustomKey == Rhs.CustomKey;
		}

		friend bool operator!=(const FInputTrip& Lhs, const FInputTrip& Rhs)
		{
			return !(Lhs == Rhs);
		}
	};

	FORCEINLINE uint32 GetTypeHash(const FInputTrip& Trip)
	{
		uint32 KeyHash = 0;
		KeyHash = HashCombine(KeyHash, ::GetTypeHash(Trip.Type));
		KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.Key));
		KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.CustomKey));
		return KeyHash;
	}
}