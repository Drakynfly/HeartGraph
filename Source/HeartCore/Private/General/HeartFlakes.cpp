// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "General/HeartFlakes.h"

#include "Compression/OodleDataCompressionUtil.h"

#if WITH_EDITOR
TAutoConsoleVariable<bool> CVarLogCompressionStatistics{
	TEXT("heart.LogCompressionStatistics"),
	false,
	TEXT("Log before/after byte counts when compressing flakes")
};
#endif

enum class EHeartMemoryObj : uint8
{
	None,

	// Direct sub-objects are serialized with us
	Internal,

	// Any other object reference is treated as a SoftObjectRef
	External,
};

FArchive& FHeartMemoryWriter::operator<<(UObject*& Obj)
{
	// Don't serialize nulls, or things we are already writing
	if (!IsValid(Obj) ||
		OuterStack.Contains(Obj))
	{
		return *this;
	}

	EHeartMemoryObj Op = EHeartMemoryObj::None;

	if (Obj->GetOuter() == OuterStack.Last())
	{
		Op = EHeartMemoryObj::Internal;
	}
	else
	{
		Op = EHeartMemoryObj::External;
	}

	*this << Op;

	switch (Op)
	{
	case EHeartMemoryObj::None:
		break;
	case EHeartMemoryObj::Internal:
		{
			FSoftClassPath Class(Obj->GetClass());
			*this << Class;

			OuterStack.Push(Obj); // Track that we are serializing this object
			Obj->Serialize(*this);
			OuterStack.Pop(); // Untrack the object
		}
		break;
	case EHeartMemoryObj::External:
		{
			FSoftObjectPath ExternalRef = Obj;
			*this << ExternalRef;
		}
		break;
	default: ;
	}

	return *this;
}

FArchive& FHeartMemoryWriter::operator<<(FObjectPtr& Obj)
{
	if (UObject* ObjPtr = Obj.Get())
	{
		*this << ObjPtr;
	}
	return *this;
}

FArchive& FHeartMemoryWriter::operator<<(FSoftObjectPtr& AssetPtr)
{
	return FArchiveUObject::SerializeSoftObjectPtr(*this, AssetPtr);
}

FArchive& FHeartMemoryWriter::operator<<(FSoftObjectPath& Value)
{
	return FArchiveUObject::SerializeSoftObjectPath(*this, Value);
}

FArchive& FHeartMemoryWriter::operator<<(FWeakObjectPtr& Value)
{
	return FArchiveUObject::SerializeWeakObjectPtr(*this, Value);
}

FString FHeartMemoryWriter::GetArchiveName() const
{
	return TEXT("FHeartMemoryWriter");
}

FArchive& FHeartMemoryReader::operator<<(UObject*& Obj)
{
	EHeartMemoryObj Op;
	*this << Op;

	switch (Op)
	{
	case EHeartMemoryObj::Internal:
		{
			FSoftClassPath Class;
			*this << Class;

			if (const UClass* ObjClass = Class.TryLoadClass<UObject>())
			{
				Obj = ObjectsCreated.Add_GetRef(NewObject<UObject>(OuterStack.Last(), ObjClass));
				OuterStack.Push(Obj);
				Obj->Serialize(*this);
				OuterStack.Pop();
			}
		}
		break;
	case EHeartMemoryObj::External:
		{
			FSoftObjectPath ExternalRef;
			*this << ExternalRef;
			Obj = ExternalRef.TryLoad();
		}
		break;
	case EHeartMemoryObj::None:
	default: return *this;
	}

	// If the outer stack is 1 here, then we are exiting the serialize loop, and can perform final options.
	if (OuterStack.Num() == 1)
	{
		if (EnumHasAnyFlags(Options, ExecPostLoad))
		{
			OuterStack[0]->PostLoad();

			for (auto&& Object : ObjectsCreated)
			{
				Object->PostLoad();
			}
		}
	}

	return *this;
}

FArchive& FHeartMemoryReader::operator<<(FObjectPtr& Obj)
{
	UObject* RawObj = nullptr;
	*this << RawObj;
	Obj = RawObj;
	return *this;
}

FArchive& FHeartMemoryReader::operator<<(FSoftObjectPtr& AssetPtr)
{
	return FArchiveUObject::SerializeSoftObjectPtr(*this, AssetPtr);
}

FArchive& FHeartMemoryReader::operator<<(FSoftObjectPath& Value)
{
	return FArchiveUObject::SerializeSoftObjectPath(*this, Value);
}

FArchive& FHeartMemoryReader::operator<<(FWeakObjectPtr& Value)
{
	return FArchiveUObject::SerializeWeakObjectPtr(*this, Value);
}

FString FHeartMemoryReader::GetArchiveName() const
{
	return TEXT("FHeartMemoryReader");
}

namespace Heart::Flakes
{
	FORCEINLINE_DEBUGGABLE void CompressFlake(FHeartFlake& Flake, const TArray<uint8>& Raw,
		const FOodleDataCompression::ECompressor Compressor,
		const FOodleDataCompression::ECompressionLevel CompressionLevel)
	{
#if WITH_EDITOR
		const auto BeforeCompression = Raw.Num();
#endif

		FOodleCompressedArray::CompressTArray(Flake.Data, Raw, Compressor, CompressionLevel);

#if WITH_EDITOR
		const auto AfterCompression = Flake.Data.Num();
		if (CVarLogCompressionStatistics.GetValueOnGameThread())
		{
			UE_LOG(LogTemp, Log, TEXT("[Flake Compression Log]: Compressed '%i' bytes to '%i' bytes"), BeforeCompression, AfterCompression);
		}
#endif
	}

	FHeartFlake CreateFlake(const FInstancedStruct& Struct, FReadOptions Options)
	{
		FHeartFlake Flake;
		Flake.Struct = Struct.GetScriptStruct()->GetPathName();

		TArray<uint8> Raw;
		FHeartMemoryWriter MemoryWriter(Raw, nullptr);
		const_cast<FInstancedStruct&>(Struct).Serialize(MemoryWriter);
		MemoryWriter.FlushCache();
		MemoryWriter.Close();

		CompressFlake(Flake, Raw, Options.Compressor, Options.CompressionLevel);

		return Flake;
	}

	FHeartFlake CreateFlake(UObject* Object, FReadOptions Options)
	{
		FHeartFlake Flake;
		Flake.Struct = Object->GetClass()->GetPathName();

		TArray<uint8> Raw;
		FHeartMemoryWriter MemoryWriter(Raw, Object);
		Object->Serialize(MemoryWriter);
		MemoryWriter.FlushCache();
		MemoryWriter.Close();

		CompressFlake(Flake, Raw, Options.Compressor, Options.CompressionLevel);

		return Flake;
	}

	void WriteStruct(FInstancedStruct& Struct, const FHeartFlake& Flake, const FWriteOptions Options)
	{
		TArray<uint8> Raw;
		FOodleCompressedArray::DecompressToTArray(Raw, Flake.Data);

		FHeartMemoryReader MemoryReader(Raw, true, nullptr, FHeartMemoryReader::None);
		Struct.Serialize(MemoryReader);
		MemoryReader.FlushCache();
		MemoryReader.Close();

		if (Options.ExecPostLoadOrPostScriptConstruct)
		{
			check(Struct.GetScriptStruct())
			Struct.GetScriptStruct()->GetCppStructOps()->PostScriptConstruct(Struct.GetMutableMemory());
		}
	}

	void WriteObject(UObject* Object, const FHeartFlake& Flake, const FWriteOptions Options)
	{
		if (!ensure(Object->IsA(Cast<UClass>(Flake.Struct.TryLoad()))))
		{
			return;
		}

		TArray<uint8> Raw;

		FOodleCompressedArray::DecompressToTArray(Raw, Flake.Data);

		FHeartMemoryReader::EOptions ReaderOptions = FHeartMemoryReader::EOptions::None;
		if (Options.ExecPostLoadOrPostScriptConstruct)
		{
			ReaderOptions |= FHeartMemoryReader::EOptions::ExecPostLoad;
		}

		FHeartMemoryReader MemoryReader(Raw, true, Object, ReaderOptions);
		Object->Serialize(MemoryReader);
		MemoryReader.FlushCache();
		MemoryReader.Close();
	}

	UObject* CreateObject(const FHeartFlake& Flake, UObject* Outer, const UClass* ExpectedClass)
	{
		if (!IsValid(ExpectedClass) || ExpectedClass->IsChildOf<AActor>())
		{
			return nullptr;
		}

		const UClass* ObjClass = LoadClass<UObject>(nullptr, *Flake.Struct.ToString(), nullptr, LOAD_None, nullptr);

		if (!IsValid(ObjClass))
		{
			return nullptr;
		}

		if (!ObjClass->IsChildOf(ExpectedClass))
		{
			return nullptr;
		}

		auto&& LoadedObject = NewObject<UObject>(Outer, ObjClass);

		WriteObject(LoadedObject, Flake);

		return LoadedObject;
	}
}

FHeartFlake UHeartFlakeLibrary::CreateFlake_Struct(const FInstancedStruct& Struct)
{
	if (Struct.IsValid())
	{
		return Heart::Flakes::CreateFlake(Struct);
	}

	return FHeartFlake();
}

FHeartFlake UHeartFlakeLibrary::CreateFlake(UObject* Object)
{
	check(Object && !Object->IsA<AActor>());

	return Heart::Flakes::CreateFlake(Object);
}

FHeartFlake_Actor UHeartFlakeLibrary::CreateFlake_Actor(AActor* Actor)
{
	check(Actor);

	FHeartFlake_Actor Flake = Heart::Flakes::CreateFlake(Actor);
	Flake.Transform = Actor->GetTransform();

	return Flake;
}

UObject* UHeartFlakeLibrary::ConstructObjectFromFlake(const FHeartFlake& Flake, UObject* Outer,
                                                      const UClass* ExpectedClass)
{
	return Heart::Flakes::CreateObject(Flake, Outer, ExpectedClass);
}

AActor* UHeartFlakeLibrary::ConstructActorFromFlake(const FHeartFlake_Actor& Flake, UObject* WorldContextObj,
	const TSubclassOf<AActor> ExpectedClass)
{
	if (!IsValid(WorldContextObj))
	{
		return nullptr;
	}

	if (!IsValid(ExpectedClass))
	{
		return nullptr;
	}

	LoadClass<AActor>(nullptr, *Flake.Struct.ToString(), nullptr, LOAD_None, nullptr);

	const TSubclassOf<AActor> ActorClass = LoadClass<AActor>(nullptr, *Flake.Struct.ToString(), nullptr, LOAD_None, nullptr);

	if (!ActorClass->IsChildOf(ExpectedClass))
	{
		return nullptr;
	}

	if (auto&& LoadedActor = WorldContextObj->GetWorld()->SpawnActorDeferred<AActor>(ActorClass, Flake.Transform))
	{
		Heart::Flakes::WriteObject(LoadedActor, Flake);

		LoadedActor->FinishSpawning(Flake.Transform);

		return LoadedActor;
	}

	return nullptr;
}