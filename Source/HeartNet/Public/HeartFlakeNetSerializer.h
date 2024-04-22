// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Serialization/HeartFlakes.h"

namespace Heart::Flakes
{
	SERIALIZATION_PROVIDER_HEADER(NetBinary)

	FHeartFlake Net_CreateFlake(const FInstancedStruct& Struct, FReadOptions Options = {});
	FHeartFlake Net_CreateFlake(const UObject* Object, FReadOptions Options = {});
	void Net_WriteStruct(FInstancedStruct& Struct, const FHeartFlake& Flake, FWriteOptions Options = {});
	void Net_WriteObject(UObject* Object, const FHeartFlake& Flake, FWriteOptions Options = {});
	FInstancedStruct Net_CreateStruct(const FHeartFlake& Flake, const UScriptStruct* ExpectedStruct);
	UObject* Net_CreateObject(const FHeartFlake& Flake, UObject* Outer, const UClass* ExpectedClass);

	template <
		typename T
		UE_REQUIRES(TModels_V<CBaseStructureProvider, T>)
	>
	FHeartFlake Net_CreateFlake(const T& Struct)
	{
		return Net_CreateFlake(FInstancedStruct::Make(Struct));
	}

	template <
		typename T
		UE_REQUIRES(TModels_V<CBaseStructureProvider, T>)
	>
	void Net_WriteStruct(T& Struct, const FHeartFlake& Flake)
	{
		if (Flake.Struct != TBaseStructure<T>::Get())
		{
			return;
		}

		FInstancedStruct InstancedStruct;
		Net_WriteStruct(InstancedStruct, Flake);
		Struct = InstancedStruct.Get<T>();
	}

	template <typename T>
	T* Net_CreateObject(const FHeartFlake& Flake, UObject* Outer = GetTransientPackage())
	{
		return Cast<T>(Net_CreateObject(Flake, Outer, T::StaticClass()));
	}
}