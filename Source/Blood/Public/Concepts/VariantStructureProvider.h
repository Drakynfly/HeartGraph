// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

class UScriptStruct;

template <typename T>
struct TVariantStructure;

/**
 * Describes a type for which TVariantStructure<T>::Get() returning a UScriptStruct* is defined.
 */
struct CVariantStructureProvider
{
	template <typename T>
	auto Requires(UScriptStruct*& StructRef) -> decltype(
		StructRef = TVariantStructure<T>::Get()
	);
};