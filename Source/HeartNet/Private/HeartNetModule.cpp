// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartNetModule.h"
#include "FlakesModule.h"
#include "Providers/FlakesNetBinarySerializer.h"

#define LOCTEXT_NAMESPACE "HeartNetModule"

void FHeartNetModule::StartupModule()
{
	FFlakesModule::Get().AddSerializationProvider(MakeUnique<Flakes::NetBinary::Type>());
}

void FHeartNetModule::ShutdownModule()
{
	FFlakesModule::Get().RemoveSerializationProvider(Flakes::NetBinary::ProviderName);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartNetModule, HeartNet)