// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"

class FBloodModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
