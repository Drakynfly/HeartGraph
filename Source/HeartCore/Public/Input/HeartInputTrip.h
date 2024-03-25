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
		FInputTrip(const FKey& Key, const bool IsControlDown, const bool IsAltDown, const bool IsShiftDown, const bool IsCommandDown, const ETripType Type)
		  : Type(Type),
			Key(Key),
			ModifierMask(EModifierKey::FromBools(IsControlDown, IsAltDown, IsShiftDown, IsCommandDown))
		{}

		FInputTrip(const FKeyEvent& KeyEvent, const ETripType Type)
		  : Type(Type),
			Key(KeyEvent.GetKey()),
			ModifierMask(EModifierKey::FromBools(KeyEvent.IsControlDown(), KeyEvent.IsAltDown(), KeyEvent.IsShiftDown(), KeyEvent.IsCommandDown()))
		{}

		FInputTrip(const FPointerEvent& PointerEvent, const ETripType Type)
		  :	Type(Type),
			Key(PointerEvent.GetEffectingButton().IsValid() ? PointerEvent.GetEffectingButton() : *PointerEvent.GetPressedButtons().CreateConstIterator()),
			ModifierMask(EModifierKey::FromBools(PointerEvent.IsControlDown(), PointerEvent.IsAltDown(), PointerEvent.IsShiftDown(), PointerEvent.IsCommandDown()))
		{}

		FInputTrip(const FName& ManualEvent)
		  : Type(Manual),
			CustomKey(ManualEvent)
		{}

		const ETripType Type = Unknown;
		const FKey Key = EKeys::Invalid;
		const uint8 ModifierMask = 0;
		const FName CustomKey = NAME_None;

		bool IsValid() const
		{
			return Type != Unknown && (Key.IsValid() || !CustomKey.IsNone());
		}

		friend bool operator==(const FInputTrip& Lhs, const FInputTrip& Rhs)
		{
			return Lhs.Type == Rhs.Type &&
				   Lhs.Key == Rhs.Key &&
				   Lhs.ModifierMask == Rhs.ModifierMask &&
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
		KeyHash = HashCombine(KeyHash, ::GetTypeHash(Trip.ModifierMask));
		KeyHash = HashCombine(KeyHash, GetTypeHash(Trip.CustomKey));
		return KeyHash;
	}
}