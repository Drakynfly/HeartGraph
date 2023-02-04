// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputTrigger.h"

TArray<FHeartWidgetInputTrip> FHeartWidgetInputTrigger_KeyDown::CreateTrips() const
{
	FHeartWidgetInputTrip Trip;
	Trip.Key = Key;
	Trip.Type = Press;
	return {Trip};
}

TArray<FHeartWidgetInputTrip> FHeartWidgetInputTrigger_KeyUp::CreateTrips() const
{
	FHeartWidgetInputTrip Trip;
	Trip.Key = Key;
	Trip.Type = Release;
	return {Trip};
}

TArray<FHeartWidgetInputTrip> FHeartWidgetInputTrigger_Manual::CreateTrips() const
{
	TArray<FHeartWidgetInputTrip> Trips;

	for (auto&& Key : Keys)
	{
		auto&& Trip = Trips.AddDefaulted_GetRef();
		Trip.Type = Manual;
		Trip.CustomKey = Key;
	}

	return Trips;
}