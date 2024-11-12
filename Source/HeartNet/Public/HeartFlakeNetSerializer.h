// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "FlakesStructs.h"

namespace Flakes
{
	/*
	 * A binary serialization provider optimized for sending data over the network. This provider assumes that the data
	 * is never written to disk. It requires about ~50% of the memory the regular binary provider uses.
	 */
	SERIALIZATION_PROVIDER_HEADER(NetBinary)

	FFlake Net_CreateFlake(const FConstStructView& Struct, FReadOptions Options = {});
	FFlake Net_CreateFlake(const UObject* Object, FReadOptions Options = {});
	void Net_WriteStruct(const FStructView& Struct, const FFlake& Flake, FWriteOptions Options = {});
	void Net_WriteObject(UObject* Object, const FFlake& Flake, FWriteOptions Options = {});
	FInstancedStruct Net_CreateStruct(const FFlake& Flake, const UScriptStruct* ExpectedStruct);
	UObject* Net_CreateObject(const FFlake& Flake, UObject* Outer, const UClass* ExpectedClass);

	template <
		typename T
		UE_REQUIRES(TModels_V<CBaseStructureProvider, T>)
	>
	FFlake Net_CreateFlake(const T& Struct)
	{
		return Net_CreateFlake(FConstStructView::Make(Struct));
	}

	template <
		typename T
		UE_REQUIRES(TModels_V<CBaseStructureProvider, T>)
	>
	void Net_WriteStruct(T& Struct, const FFlake& Flake)
	{
		if (Flake.Struct != TBaseStructure<T>::Get())
		{
			return;
		}

		Net_WriteStruct(FStructView::Make(Struct), Flake);
	}

	template <typename T>
	T* Net_CreateObject(const FFlake& Flake, UObject* Outer = GetTransientPackage())
	{
		return Cast<T>(Net_CreateObject(Flake, Outer, T::StaticClass()));
	}
}