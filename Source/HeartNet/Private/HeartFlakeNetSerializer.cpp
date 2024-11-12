// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartFlakeNetSerializer.h"
#include "FlakesMemory.h"
#include "StructView.h"

namespace Flakes
{
	// Configures an archive to be optimized for sending data over the network.
	void ConfigureNetArchive(FArchive& Ar)
	{
		Ar.ArNoDelta = false;
		Ar.SetIsPersistent(false);
		Ar.ArIsNetArchive = true;
		Ar.SetUseUnversionedPropertySerialization(true);
	}

	void FSerializationProvider_NetBinary::Static_ReadData(const FConstStructView& Struct, TArray<uint8>& OutData)
	{
		FRecursiveMemoryWriter MemoryWriter(OutData, nullptr);
		ConfigureNetArchive(MemoryWriter);
		// For some reason, SerializeItem is not const, so we have to const_cast the ScriptStruct
		// We also have to const_cast the memory because *we* know that this function only reads from it, but
		// SerializeItem is a bidirectional serializer, so it doesn't.
		const_cast<UScriptStruct*>(Struct.GetScriptStruct())->SerializeItem(MemoryWriter,
			const_cast<uint8*>(Struct.GetMemory()), nullptr);
		MemoryWriter.FlushCache();
		MemoryWriter.Close();
	}

	void FSerializationProvider_NetBinary::Static_ReadData(const UObject* Object, TArray<uint8>& OutData)
	{
		FRecursiveMemoryWriter MemoryWriter(OutData, Object);
		ConfigureNetArchive(MemoryWriter);
		const_cast<UObject*>(Object)->Serialize(MemoryWriter);
		MemoryWriter.FlushCache();
		MemoryWriter.Close();
	}

	void FSerializationProvider_NetBinary::Static_WriteData(const FStructView& Struct, const TArray<uint8>& Data)
	{
		FRecursiveMemoryReader MemoryReader(Data, false, nullptr);
		ConfigureNetArchive(MemoryReader);
		// For some reason, SerializeItem is not const, so we have to const_cast the ScriptStruct
		const_cast<UScriptStruct*>(Struct.GetScriptStruct())->SerializeItem(MemoryReader, Struct.GetMemory(), nullptr);
		MemoryReader.FlushCache();
		MemoryReader.Close();
	}

	void FSerializationProvider_NetBinary::Static_WriteData(UObject* Object, const TArray<uint8>& Data)
	{
		FRecursiveMemoryReader MemoryReader(Data, false, Object);
		ConfigureNetArchive(MemoryReader);
		Object->Serialize(MemoryReader);
		MemoryReader.FlushCache();
		MemoryReader.Close();
	}

	FName FSerializationProvider_NetBinary::GetProviderName()
	{
		static const FLazyName NetBinarySerializationProvider("NetBinary");
		return NetBinarySerializationProvider;
	}

#define ENABLE_NET_SERIALIZER 1

	FFlake Net_CreateFlake(const FConstStructView& Struct, const FReadOptions Options)
	{
#if ENABLE_NET_SERIALIZER
		return CreateFlake<FSerializationProvider_NetBinary>(Struct, Options);
#else
		return CreateFlake(Struct, Options);
#endif
	}

	FFlake Net_CreateFlake(const UObject* Object, const FReadOptions Options)
	{
#if ENABLE_NET_SERIALIZER
		return CreateFlake<FSerializationProvider_NetBinary>(Object, Options);
#else
		return CreateFlake(Object, Options);
#endif
	}

	void Net_WriteStruct(const FStructView& Struct, const FFlake& Flake, const FWriteOptions Options)
	{
#if ENABLE_NET_SERIALIZER
		WriteStruct<FSerializationProvider_NetBinary>(Struct, Flake, Options);
#else
		WriteStruct(Struct, Flake);
#endif
	}

	void Net_WriteObject(UObject* Object, const FFlake& Flake, const FWriteOptions Options)
	{
#if ENABLE_NET_SERIALIZER
		WriteObject<FSerializationProvider_NetBinary>(Object, Flake, Options);
#else
		WriteObject(Object, Flake);
#endif
	}

	FInstancedStruct Net_CreateStruct(const FFlake& Flake, const UScriptStruct* ExpectedStruct)
	{
#if ENABLE_NET_SERIALIZER
		return CreateStruct<FSerializationProvider_NetBinary>(Flake, ExpectedStruct);
#else
		return CreateStruct(Flake, ExpectedStruct);
#endif
	}

	UObject* Net_CreateObject(const FFlake& Flake, UObject* Outer, const UClass* ExpectedClass)
	{
#if ENABLE_NET_SERIALIZER
		return CreateObject<FSerializationProvider_NetBinary>(Flake, Outer, ExpectedClass);
#else
		return CreateObject(Flake, Outer, ExpectedClass);
#endif
	}
}