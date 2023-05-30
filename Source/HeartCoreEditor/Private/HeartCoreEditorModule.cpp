// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartCoreEditorModule.h"

DEFINE_LOG_CATEGORY(LogHeartCoreEditor);

#define LOCTEXT_NAMESPACE "HeartCoreEditorModule"

void FHeartCoreEditorModule::StartupModule()
{
}

void FHeartCoreEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartCoreEditorModule, HeartCoreEditor)