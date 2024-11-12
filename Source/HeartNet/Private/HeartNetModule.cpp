// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartNetModule.h"
#include "FlakesModule.h"
#include "HeartFlakeNetSerializer.h"

#define LOCTEXT_NAMESPACE "HeartNetModule"

void FHeartNetModule::StartupModule()
{
	FFlakesModule::Get().AddSerializationProvider(MakeUnique<Flakes::FSerializationProvider_NetBinary>());
}

void FHeartNetModule::ShutdownModule()
{
	FFlakesModule::Get().RemoveSerializationProvider(Flakes::FSerializationProvider_NetBinary().GetProviderName());
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartNetModule, HeartNet)