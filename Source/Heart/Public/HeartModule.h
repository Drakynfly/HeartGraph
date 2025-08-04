// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"

class FHeartModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
