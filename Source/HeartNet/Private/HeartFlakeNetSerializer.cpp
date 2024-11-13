// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartFlakeNetSerializer.h"
#include "FlakesMemory.h"
#include "StructUtils/StructView.h"

namespace Flakes::NetBinary
{
	// Configures an archive to be optimized for sending data over the network.
	void ConfigureNetArchive(FArchive& Ar)
	{
		Ar.ArNoDelta = false;
		Ar.SetIsPersistent(false);
		Ar.ArIsNetArchive = true;
		Ar.SetUseUnversionedPropertySerialization(true);
	}

	void FSerializationProvider_NetBinary::ReadData(const FConstStructView& Struct, TArray<uint8>& OutData, const UObject* Outer)
	{
		FRecursiveMemoryWriter MemoryWriter(OutData, Outer);
		ConfigureNetArchive(MemoryWriter);
		// For some reason, SerializeItem is not const, so we have to const_cast the ScriptStruct
		// We also have to const_cast the memory because *we* know that this function only reads from it, but
		// SerializeItem is a bidirectional serializer, so it doesn't.
		const_cast<UScriptStruct*>(Struct.GetScriptStruct())->SerializeItem(MemoryWriter,
			const_cast<uint8*>(Struct.GetMemory()), nullptr);
		MemoryWriter.FlushCache();
		MemoryWriter.Close();
	}

	void FSerializationProvider_NetBinary::ReadData(const UObject* Object, TArray<uint8>& OutData)
	{
		FRecursiveMemoryWriter MemoryWriter(OutData, Object);
		ConfigureNetArchive(MemoryWriter);
		const_cast<UObject*>(Object)->Serialize(MemoryWriter);
		MemoryWriter.FlushCache();
		MemoryWriter.Close();
	}

	void FSerializationProvider_NetBinary::WriteData(const FStructView& Struct, const TArray<uint8>& Data, UObject* Outer)
	{
		FRecursiveMemoryReader MemoryReader(Data, false, Outer);
		ConfigureNetArchive(MemoryReader);
		// For some reason, SerializeItem is not const, so we have to const_cast the ScriptStruct
		const_cast<UScriptStruct*>(Struct.GetScriptStruct())->SerializeItem(MemoryReader, Struct.GetMemory(), nullptr);
		MemoryReader.FlushCache();
		MemoryReader.Close();
	}

	void FSerializationProvider_NetBinary::WriteData(UObject* Object, const TArray<uint8>& Data)
	{
		FRecursiveMemoryReader MemoryReader(Data, false, Object);
		ConfigureNetArchive(MemoryReader);
		Object->Serialize(MemoryReader);
		MemoryReader.FlushCache();
		MemoryReader.Close();
	}
}