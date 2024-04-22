// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartFlakeNetSerializer.h"

namespace Heart::Flakes
{
	// Configures an archive to be optimized for sending data over the network.
	void ConfigureNetArchive(FArchive& Ar)
	{
		Ar.ArNoDelta = false;
		Ar.SetIsPersistent(false);
		Ar.ArIsNetArchive = true;
		Ar.SetUseUnversionedPropertySerialization(true);
	}

	void FSerializationProvider_NetBinary::Static_ReadData(const FInstancedStruct& Struct, TArray<uint8>& OutData)
	{
		FRecursiveMemoryWriter MemoryWriter(OutData, nullptr);
		ConfigureNetArchive(MemoryWriter);
		bool Success;
		const_cast<FInstancedStruct&>(Struct).NetSerialize(MemoryWriter, nullptr, Success);
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

	void FSerializationProvider_NetBinary::Static_WriteData(FInstancedStruct& Struct, const TArray<uint8>& Data)
	{
		FRecursiveMemoryReader MemoryReader(Data, false, nullptr);
		ConfigureNetArchive(MemoryReader);
		bool Success;
		Struct.NetSerialize(MemoryReader, nullptr, Success);
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

	FHeartFlake Net_CreateFlake(const FInstancedStruct& Struct, const FReadOptions Options)
	{
#if ENABLE_NET_SERIALIZER
		return CreateFlake<FSerializationProvider_NetBinary>(Struct, Options);
#else
		return CreateFlake(Struct, Options);
#endif
	}

	FHeartFlake Net_CreateFlake(const UObject* Object, const FReadOptions Options)
	{
#if ENABLE_NET_SERIALIZER
		return CreateFlake<FSerializationProvider_NetBinary>(Object, Options);
#else
		return CreateFlake(Object, Options);
#endif
	}

	void Net_WriteStruct(FInstancedStruct& Struct, const FHeartFlake& Flake, const FWriteOptions Options)
	{
#if ENABLE_NET_SERIALIZER
		WriteStruct<FSerializationProvider_NetBinary>(Struct, Flake, Options);
#else
		WriteStruct(Struct, Flake);
#endif
	}

	void Net_WriteObject(UObject* Object, const FHeartFlake& Flake, const FWriteOptions Options)
	{
#if ENABLE_NET_SERIALIZER
		WriteObject<FSerializationProvider_NetBinary>(Object, Flake, Options);
#else
		WriteObject(Object, Flake);
#endif
	}

	FInstancedStruct Net_CreateStruct(const FHeartFlake& Flake, const UScriptStruct* ExpectedStruct)
	{
#if ENABLE_NET_SERIALIZER
		return CreateStruct<FSerializationProvider_NetBinary>(Flake, ExpectedStruct);
#else
		return CreateStruct(Flake, ExpectedStruct);
#endif
	}

	UObject* Net_CreateObject(const FHeartFlake& Flake, UObject* Outer, const UClass* ExpectedClass)
	{
#if ENABLE_NET_SERIALIZER
		return CreateObject<FSerializationProvider_NetBinary>(Flake, Outer, ExpectedClass);
#else
		return CreateObject(Flake, Outer, ExpectedClass);
#endif
	}
}