#pragma once

#include "Modules/ModuleManager.h"

class FHeartCoreEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
