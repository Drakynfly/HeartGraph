// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

struct FHeartWidgetInputTrip
{
	FKey Key = EKeys::Invalid;
	bool Release = false;

	friend bool operator==(const FHeartWidgetInputTrip& Lhs, const FHeartWidgetInputTrip& Rhs)
	{
		return Lhs.Key == Rhs.Key && Lhs.Release == Rhs.Release;
	}

	friend bool operator!=(const FHeartWidgetInputTrip& Lhs, const FHeartWidgetInputTrip& Rhs)
	{
		return !(Lhs == Rhs);
	}
};

FORCEINLINE uint32 GetTypeHash(const FHeartWidgetInputTrip& Trip)
{
	uint32 KeyHash = 0;
	KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.Key));
	KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.Release));
	//KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.PointerEvent.Get(FPointerEvent())));
	//KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.KeyEvent.Get(FKeyEvent())));
	return KeyHash;
}