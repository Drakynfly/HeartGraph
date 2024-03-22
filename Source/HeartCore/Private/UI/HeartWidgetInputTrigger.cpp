// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputTrigger.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartWidgetInputTrigger)

TArray<Heart::Input::FInputTrip> FHeartWidgetInputTrigger_KeyDown::CreateTrips() const
{
	TArray<Heart::Input::FInputTrip> Trips;

	for (const FKey& TripKey : Keys)
	{
		Heart::Input::FInputTrip& Trip = Trips.AddDefaulted_GetRef();
		Trip.Key = TripKey;
		Trip.Type = Heart::Input::Press;
	}

	return Trips;
}

TArray<Heart::Input::FInputTrip> FHeartWidgetInputTrigger_KeyDownMod::CreateTrips() const
{
	TArray<Heart::Input::FInputTrip> Trips;

	for (const auto& TripKey : Keys)
	{
		Heart::Input::FInputTrip& Trip = Trips.AddDefaulted_GetRef();
		Trip.Key = TripKey.Key;
		Trip.ModifierMask = EModifierKey::FromBools(TripKey.WithControl, TripKey.WithAlt, TripKey.WithShift, TripKey.WithCommand);
		Trip.Type = Heart::Input::Press;
	}

	return Trips;
}

TArray<Heart::Input::FInputTrip> FHeartWidgetInputTrigger_KeyUp::CreateTrips() const
{
	TArray<Heart::Input::FInputTrip> Trips;

	for (const FKey& TripKey : Keys)
	{
		Heart::Input::FInputTrip& Trip = Trips.AddDefaulted_GetRef();
		Trip.Key = TripKey;
		Trip.Type = Heart::Input::Release;
	}

	return Trips;
}

TArray<Heart::Input::FInputTrip> FHeartWidgetInputTrigger_Manual::CreateTrips() const
{
	TArray<Heart::Input::FInputTrip> Trips;

	for (const FName Key : Keys)
	{
		Heart::Input::FInputTrip& Trip = Trips.AddDefaulted_GetRef();
		Trip.Type = Heart::Input::Manual;
		Trip.CustomKey = Key;
	}

	return Trips;
}