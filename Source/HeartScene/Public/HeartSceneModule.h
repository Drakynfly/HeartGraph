// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHeartGraphScene, Log, All)

class FHeartSceneModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
