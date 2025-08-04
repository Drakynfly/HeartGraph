// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"

class FHeartCoreModule : public IModuleInterface
{
public:
    HEARTCORE_API static FHeartCoreModule& Get();

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};