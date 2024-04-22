// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartNetModule.h"
#include "HeartCoreModule.h"
#include "HeartFlakeNetSerializer.h"

#define LOCTEXT_NAMESPACE "HeartNetModule"

void FHeartNetModule::StartupModule()
{
	FHeartCoreModule::Get().AddSerializationProvider(MakeUnique<Heart::Flakes::FSerializationProvider_NetBinary>());
}

void FHeartNetModule::ShutdownModule()
{
	FHeartCoreModule::Get().RemoveSerializationProvider(Heart::Flakes::FSerializationProvider_NetBinary().GetProviderName());
\
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartNetModule, HeartNet)