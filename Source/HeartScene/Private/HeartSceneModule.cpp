// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartSceneModule.h"

#define LOCTEXT_NAMESPACE "HeartSceneModule"

DEFINE_LOG_CATEGORY(LogHeartGraphScene)

void FHeartSceneModule::StartupModule()
{
}

void FHeartSceneModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartSceneModule, HeartScene)