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
	AddSerializationProvider(MakeUnique<Heart::Flakes::FSerializationProvider_Binary>());
}

void FHeartCoreModule::ShutdownModule()
{
}

TArray<FName> FHeartCoreModule::GetAllProviderNames() const
{
	TArray<FName> Out;
	SerializationProviders.GetKeys(Out);
	return Out;
}

void FHeartCoreModule::AddSerializationProvider(TUniquePtr<Heart::Flakes::ISerializationProvider>&& Provider)
{
	SerializationProviders.Add(Provider->GetProviderName(), MoveTemp(Provider));
}

void FHeartCoreModule::RemoveSerializationProvider(const FName ProviderName)
{
	SerializationProviders.Remove(ProviderName);
}

void FHeartCoreModule::UseSerializationProvider(const FName ProviderName, const FSerializationProviderExec& Exec) const
{
	if (const TUniquePtr<Heart::Flakes::ISerializationProvider>* Found = SerializationProviders.Find(ProviderName))
	{
		Exec(Found->Get());
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHeartCoreModule, HeartCore)