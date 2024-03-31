// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartJsonModule.h"
#include "HeartCoreModule.h"
#include "HeartJsonSerializer.h"

#define LOCTEXT_NAMESPACE "FHeartJsonModule"

void FHeartJsonModule::StartupModule()
{
	FHeartCoreModule::Get().AddSerializationProvider(MakeUnique<Heart::Flakes::FSerializationProvider_Json>());
}

void FHeartJsonModule::ShutdownModule()
{
	FHeartCoreModule::Get().RemoveSerializationProvider(Heart::Flakes::FSerializationProvider_Json().GetProviderName());
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartJsonModule, HeartJson)