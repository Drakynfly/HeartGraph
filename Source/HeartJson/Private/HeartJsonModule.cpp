// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartJsonModule.h"
#include "FlakesModule.h"
#include "HeartJsonSerializer.h"

#define LOCTEXT_NAMESPACE "HeartJsonModule"

void FHeartJsonModule::StartupModule()
{
	FFlakesModule::Get().AddSerializationProvider(MakeUnique<Flakes::Json::Type>());
}

void FHeartJsonModule::ShutdownModule()
{
	FFlakesModule::Get().RemoveSerializationProvider(Flakes::Json::Type().GetProviderName());
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartJsonModule, HeartJson)