// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartInputTrigger.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartInputTrigger)

TArray<Heart::Input::FInputTrip> FHeartInputTrigger_KeyDown::CreateTrips() const
{
	TArray<Heart::Input::FInputTrip> Trips;

	for (const FKey& TripKey : Keys)
	{
		Trips.Emplace(TripKey, false, false, false, false, Heart::Input::Press);
	}

	return Trips;
}

TArray<Heart::Input::FInputTrip> FHeartInputTrigger_KeyDownMod::CreateTrips() const
{
	TArray<Heart::Input::FInputTrip> Trips;

	for (const auto& TripKey : Keys)
	{
		Trips.Emplace(TripKey.Key, TripKey.WithControl, TripKey.WithAlt, TripKey.WithShift, TripKey.WithCommand, Heart::Input::Press);
	}

	return Trips;
}

TArray<Heart::Input::FInputTrip> FHeartInputTrigger_KeyUp::CreateTrips() const
{
	TArray<Heart::Input::FInputTrip> Trips;

	for (const FKey& TripKey : Keys)
	{
		Trips.Emplace(TripKey, false, false, false, false, Heart::Input::Release);
	}

	return Trips;
}

TArray<Heart::Input::FInputTrip> FHeartInputTrigger_KeyUpMod::CreateTrips() const
{
	TArray<Heart::Input::FInputTrip> Trips;

	for (const auto& TripKey : Keys)
	{
		Trips.Emplace(TripKey.Key, TripKey.WithControl, TripKey.WithAlt, TripKey.WithShift, TripKey.WithCommand, Heart::Input::Release);
	}

	return Trips;
}

TArray<Heart::Input::FInputTrip> FHeartInputTrigger_Manual::CreateTrips() const
{
	TArray<Heart::Input::FInputTrip> Trips;

	for (const FName TripKey : Keys)
	{
		Trips.Emplace(TripKey);
	}

	return Trips;
}