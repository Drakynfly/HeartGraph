// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartCoreModule.h"

#define LOCTEXT_NAMESPACE "HeartCoreModule"

const static FLazyName ModuleName("HeartCore");

FHeartCoreModule& FHeartCoreModule::Get()
{
	return FModuleManager::Get().GetModuleChecked<FHeartCoreModule>(ModuleName);
}

void FHeartCoreModule::StartupModule()
{
}

void FHeartCoreModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartCoreModule, HeartCore)