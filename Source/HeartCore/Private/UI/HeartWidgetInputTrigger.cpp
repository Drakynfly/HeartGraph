// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputTrigger.h"

TArray<FHeartWidgetInputTrip> FHeartWidgetInputTrigger_KeyDown::CreateTrips() const
{
	TArray<FHeartWidgetInputTrip> Trips;

	for (const FKey& TripKey : Keys)
	{
		FHeartWidgetInputTrip& Trip = Trips.AddDefaulted_GetRef();
		Trip.Key = TripKey;
		Trip.Type = Press;
	}

	return Trips;
}

TArray<FHeartWidgetInputTrip> FHeartWidgetInputTrigger_KeyUp::CreateTrips() const
{
	TArray<FHeartWidgetInputTrip> Trips;

	for (const FKey& TripKey : Keys)
	{
		FHeartWidgetInputTrip& Trip = Trips.AddDefaulted_GetRef();
		Trip.Key = TripKey;
		Trip.Type = Release;
	}

	return Trips;
}

TArray<FHeartWidgetInputTrip> FHeartWidgetInputTrigger_Manual::CreateTrips() const
{
	TArray<FHeartWidgetInputTrip> Trips;

	for (const FName Key : Keys)
	{
		FHeartWidgetInputTrip& Trip = Trips.AddDefaulted_GetRef();
		Trip.Type = Manual;
		Trip.CustomKey = Key;
	}

	return Trips;
}