// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InputCoreTypes.h"
#include "GameFramework/PlayerInput.h"

namespace Heart::Input
{
	enum ETripType
	{
		Unknown,
		Press,
		Release,
		Manual
	};

	// Convert Input Event to less granular TripType
	FORCEINLINE ETripType InputEventToTripType(const EInputEvent Event)
	{
		switch (Event)
		{
			// All these are compressed to Press
		case IE_Pressed:
		case IE_Repeat:
		case IE_DoubleClick:
			return ETripType::Press;

			// Direct mapping
		case IE_Released:
			return ETripType::Release;

			// Unhandled cases
		case IE_Axis:
		case IE_MAX:
		default:
			return ETripType::Unknown;
		}
	}

	FORCEINLINE EModifierKey::Type ModifierKeysFromState(const FModifierKeysState& State)
	{
		EModifierKey::Type ModifierMask = EModifierKey::None;
		if (State.IsControlDown())	ModifierMask |= EModifierKey::Control;
		if (State.IsAltDown())		ModifierMask |= EModifierKey::Alt;
		if (State.IsShiftDown())	ModifierMask |= EModifierKey::Shift;
		if (State.IsCommandDown())	ModifierMask |= EModifierKey::Command;

		return ModifierMask;
	}

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

		FInputTrip(const FInputKeyParams& Params)
		  : Type(InputEventToTripType(Params.Event)),
		    Key(Params.Key),
		    ModifierMask(ModifierKeysFromState(FSlateApplication::Get().GetModifierKeys()))
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