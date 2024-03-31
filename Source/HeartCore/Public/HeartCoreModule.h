// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Serialization/HeartFlakes.h"

class FHeartCoreModule : public IModuleInterface
{
public:
    HEARTCORE_API static FHeartCoreModule& Get();

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;


    /**     HEART FLAKE SERIALIZATION API    **/

    TArray<FName> GetAllProviderNames() const;

    HEARTCORE_API void AddSerializationProvider(TUniquePtr<Heart::Flakes::ISerializationProvider>&& Provider);
    HEARTCORE_API void RemoveSerializationProvider(FName ProviderName);

    using FSerializationProviderExec = TFunctionRef<void(Heart::Flakes::ISerializationProvider*)>;
    void UseSerializationProvider(FName ProviderName, const FSerializationProviderExec& Exec) const;

private:
    TMap<FName, TUniquePtr<Heart::Flakes::ISerializationProvider>> SerializationProviders;
};