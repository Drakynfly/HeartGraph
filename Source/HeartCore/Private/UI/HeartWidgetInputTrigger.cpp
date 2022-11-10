// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetInputTrigger.h"

FHeartWidgetInputTrip FHeartWidgetInputTrigger_KeyDown::CreateTrip() const
{
	FHeartWidgetInputTrip Trip;
	Trip.Key = Key;
	Trip.Release = false;
	//Trip.KeyEvent = Heart::Input::MakeKeyEventFromKey(Key);
	return Trip;
}

FHeartWidgetInputTrip FHeartWidgetInputTrigger_KeyUp::CreateTrip() const
{
	FHeartWidgetInputTrip Trip;
	Trip.Key = Key;
	Trip.Release = true;
	//Trip.KeyEvent = Heart::Input::MakeKeyEventFromKey(Key);
	return Trip;
}