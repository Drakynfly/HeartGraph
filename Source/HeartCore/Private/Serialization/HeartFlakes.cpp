// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Serialization/HeartFlakes.h"
#include "HeartCoreModule.h"

#include "Compression/OodleDataCompressionUtil.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartFlakes)

DEFINE_LOG_CATEGORY(LogFlakes)

#if WITH_EDITOR
TAutoConsoleVariable<bool> CVarLogCompressionStatistics{
	TEXT("heart.LogCompressionStatistics"),
	false,
	TEXT("Log before/after byte counts when compressing flakes")
};
#endif

namespace Heart::Flakes
{
	enum class ERecursiveMemoryObj : uint8
	{
		None,

		// Direct sub-objects are serialized with us
		Internal,

		// Any other object reference is treated as a SoftObjectRef
		External,
	};

	FArchive& FRecursiveMemoryWriter::operator<<(UObject*& Obj)
	{
		// Don't serialize nulls, or things we are already writing
		if (!IsValid(Obj) ||
			OuterStack.Contains(Obj))
		{
			return *this;
		}

		ERecursiveMemoryObj Op = ERecursiveMemoryObj::None;

		if (Obj->GetOuter() == OuterStack.Last())
		{
			Op = ERecursiveMemoryObj::Internal;
		}
		else
		{
			Op = ERecursiveMemoryObj::External;
		}

		*this << Op;

		switch (Op)
		{
		case ERecursiveMemoryObj::None:
			break;
		case ERecursiveMemoryObj::Internal:
			{
				FSoftClassPath Class(Obj->GetClass());
				*this << Class;

				OuterStack.Push(Obj); // Track that we are serializing this object
				Obj->Serialize(*this);
				OuterStack.Pop(); // Untrack the object
			}
			break;
		case ERecursiveMemoryObj::External:
			{
				FSoftObjectPath ExternalRef = Obj;
				*this << ExternalRef;
			}
			break;
		default: ;
		}

		return *this;
	}

	FArchive& FRecursiveMemoryWriter::operator<<(FObjectPtr& Obj)
	{
		if (UObject* ObjPtr = Obj.Get())
		{
			*this << ObjPtr;
		}
		return *this;
	}

	FArchive& FRecursiveMemoryWriter::operator<<(FSoftObjectPtr& AssetPtr)
	{
		return FArchiveUObject::SerializeSoftObjectPtr(*this, AssetPtr);
	}

	FArchive& FRecursiveMemoryWriter::operator<<(FSoftObjectPath& Value)
	{
		return FArchiveUObject::SerializeSoftObjectPath(*this, Value);
	}

	FArchive& FRecursiveMemoryWriter::operator<<(FWeakObjectPtr& Value)
	{
		return FArchiveUObject::SerializeWeakObjectPtr(*this, Value);
	}

	FString FRecursiveMemoryWriter::GetArchiveName() const
	{
		return TEXT("FRecursiveMemoryWriter");
	}

	FArchive& FRecursiveMemoryReader::operator<<(UObject*& Obj)
	{
		ERecursiveMemoryObj Op;
		*this << Op;

		switch (Op)
		{
		case ERecursiveMemoryObj::Internal:
			{
				FSoftClassPath Class;
				*this << Class;

				if (const UClass* ObjClass = Class.TryLoadClass<UObject>())
				{
					Obj = NewObject<UObject>(OuterStack.Last(), ObjClass);
					OuterStack.Push(Obj);
					Obj->Serialize(*this);
					OuterStack.Pop();
				}
			}
			break;
		case ERecursiveMemoryObj::External:
			{
				FSoftObjectPath ExternalRef;
				*this << ExternalRef;
				Obj = ExternalRef.TryLoad();
			}
			break;
		case ERecursiveMemoryObj::None:
		default: return *this;
		}

		return *this;
	}

	FArchive& FRecursiveMemoryReader::operator<<(FObjectPtr& Obj)
	{
		UObject* RawObj = nullptr;
		*this << RawObj;
		Obj = RawObj;
		return *this;
	}

	FArchive& FRecursiveMemoryReader::operator<<(FSoftObjectPtr& AssetPtr)
	{
		return FArchiveUObject::SerializeSoftObjectPtr(*this, AssetPtr);
	}

	FArchive& FRecursiveMemoryReader::operator<<(FSoftObjectPath& Value)
	{
		return FArchiveUObject::SerializeSoftObjectPath(*this, Value);
	}

	FArchive& FRecursiveMemoryReader::operator<<(FWeakObjectPtr& Value)
	{
		return FArchiveUObject::SerializeWeakObjectPtr(*this, Value);
	}

	FString FRecursiveMemoryReader::GetArchiveName() const
	{
		return TEXT("FRecursiveMemoryReader");
	}

	namespace Private
	{
		void CompressFlake(FHeartFlake& Flake, const TArray<uint8>& Raw,
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
				UE_LOG(LogFlakes, Log, TEXT("[Flake Compression Log]: Compressed '%i' bytes to '%i' bytes"), BeforeCompression, AfterCompression);
			}
#endif
		}

		void DecompressFlake(const FHeartFlake& Flake, TArray<uint8>& Raw)
		{
			FOodleCompressedArray::DecompressToTArray(Raw, Flake.Data);
		}

		void PostLoadStruct(const FStructView& Struct)
		{
			check(Struct.GetScriptStruct())
			Struct.GetScriptStruct()->GetCppStructOps()->PostScriptConstruct(Struct.GetMemory());
		}

		void PostLoadUObject(UObject* Object)
		{
			Object->PostLoad();
			ForEachObjectWithOuter(Object,
				[](UObject* Subobject)
				{
					Subobject->PostLoad();
				});
		}
	}

	FHeartFlake CreateFlake(const FConstStructView& Struct, const FReadOptions Options)
	{
		return CreateFlake<FSerializationProvider_Binary>(Struct, Options);
	}

	FHeartFlake CreateFlake(const UObject* Object, const FReadOptions Options)
	{
		return CreateFlake<FSerializationProvider_Binary>(Object, Options);
	}

	void WriteStruct(const FStructView& Struct, const FHeartFlake& Flake)
	{
		WriteStruct<FSerializationProvider_Binary>(Struct, Flake);
	}

	void WriteObject(UObject* Object, const FHeartFlake& Flake)
	{
		WriteObject<FSerializationProvider_Binary>(Object, Flake);
	}

	FInstancedStruct CreateStruct(const FHeartFlake& Flake, const UScriptStruct* ExpectedStruct)
	{
		return CreateStruct<FSerializationProvider_Binary>(Flake, ExpectedStruct);
	}

	UObject* CreateObject(const FHeartFlake& Flake, UObject* Outer, const UClass* ExpectedClass)
	{
		return CreateObject<FSerializationProvider_Binary>(Flake, Outer, ExpectedClass);
	}

	void FSerializationProvider_Binary::Static_ReadData(const FConstStructView& Struct, TArray<uint8>& OutData)
	{
		FRecursiveMemoryWriter MemoryWriter(OutData, nullptr);
		// For some reason, SerializeItem is not const, so we have to const_cast the ScriptStruct
		// We also have to const_cast the memory because *we* know that this function only reads from it, but
		// SerializeItem is a bidirectional serializer, so it doesn't.
		const_cast<UScriptStruct*>(Struct.GetScriptStruct())->SerializeItem(MemoryWriter,
			const_cast<uint8*>(Struct.GetMemory()), nullptr);
		MemoryWriter.FlushCache();
		MemoryWriter.Close();
	}

	void FSerializationProvider_Binary::Static_ReadData(const UObject* Object, TArray<uint8>& OutData)
	{
		FRecursiveMemoryWriter MemoryWriter(OutData, Object);
		const_cast<UObject*>(Object)->Serialize(MemoryWriter);
		MemoryWriter.FlushCache();
		MemoryWriter.Close();
	}

	void FSerializationProvider_Binary::Static_WriteData(const FStructView& Struct, const TArray<uint8>& Data)
	{
		FRecursiveMemoryReader MemoryReader(Data, true, nullptr);
		// For some reason, SerializeItem is not const, so we have to const_cast the ScriptStruct
		const_cast<UScriptStruct*>(Struct.GetScriptStruct())->SerializeItem(MemoryReader, Struct.GetMemory(), nullptr);
		MemoryReader.FlushCache();
		MemoryReader.Close();
	}

	void FSerializationProvider_Binary::Static_WriteData(UObject* Object, const TArray<uint8>& Data)
	{
		FRecursiveMemoryReader MemoryReader(Data, true, Object);
		Object->Serialize(MemoryReader);
		MemoryReader.FlushCache();
		MemoryReader.Close();
	}

	FName FSerializationProvider_Binary::GetProviderName()
	{
		static const FLazyName BinarySerializationProvider("Binary");
		return BinarySerializationProvider;
	}

	FHeartFlake CreateFlake(const FName Serializer, const FConstStructView& Struct, const FReadOptions Options)
	{
		TArray<uint8> Raw;

		if (Struct.IsValid())
		{
			FHeartCoreModule::Get().UseSerializationProvider(Serializer,
				[&](ISerializationProvider* Provider)
				{
					Provider->ReadData(Struct, Raw);
				});
		}

		FHeartFlake Flake;
		Flake.Struct = Struct.GetScriptStruct();

		Private::CompressFlake(Flake, Raw, Options.Compressor, Options.CompressionLevel);

		return Flake;
	}

	FHeartFlake CreateFlake(const FName Serializer, const UObject* Object, const FReadOptions Options)
	{
		check(Object && !Object->IsA<AActor>());

		FHeartFlake Flake;
		Flake.Struct = Object->GetClass();

		TArray<uint8> Raw;

		FHeartCoreModule::Get().UseSerializationProvider(Serializer,
			[&](ISerializationProvider* Provider)
			{
				Provider->ReadData(Object, Raw);
			});

		Private::CompressFlake(Flake, Raw, Options.Compressor, Options.CompressionLevel);

		return Flake;
	}

	void WriteStruct(const FName Serializer, const FStructView& Struct, const FHeartFlake& Flake, const FWriteOptions Options)
	{
		TArray<uint8> Raw;
		Private::DecompressFlake(Flake, Raw);

		FHeartCoreModule::Get().UseSerializationProvider(Serializer,
			[&](ISerializationProvider* Provider)
			{
				Provider->WriteData(Struct, Raw);
			});

		if (Options.ExecPostLoadOrPostScriptConstruct)
		{
			Private::PostLoadStruct(Struct);
		}
	}

	void WriteObject(const FName Serializer, UObject* Object, const FHeartFlake& Flake, const FWriteOptions Options)
	{
		TArray<uint8> Raw;
		Private::DecompressFlake(Flake, Raw);

		FHeartCoreModule::Get().UseSerializationProvider(Serializer,
			[&](ISerializationProvider* Provider)
			{
				Provider->WriteData(Object, Raw);
			});

		if (Options.ExecPostLoadOrPostScriptConstruct)
		{
			Private::PostLoadUObject(Object);
		}
	}

	FInstancedStruct CreateStruct(const FName Serializer, const FHeartFlake& Flake, const UScriptStruct* ExpectedStruct)
	{
		if (!IsValid(ExpectedStruct))
		{
			return {};
		}

		const UScriptStruct* StructType = Cast<UScriptStruct>(Flake.Struct.TryLoad());

		if (!IsValid(StructType))
		{
			return {};
		}

		if (!StructType->IsChildOf(ExpectedStruct))
		{
			return {};
		}

		FInstancedStruct CreatedStruct;
		CreatedStruct.InitializeAs(StructType);

		FWriteOptions Options;
		Options.ExecPostLoadOrPostScriptConstruct = true;

		WriteStruct(Serializer, CreatedStruct, Flake, Options);

		return CreatedStruct;
	}

	UObject* CreateObject(const FName Serializer, const FHeartFlake& Flake, UObject* Outer, const UClass* ExpectedClass)
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

		FWriteOptions Options;
		Options.ExecPostLoadOrPostScriptConstruct = true;

		WriteObject(Serializer, LoadedObject, Flake, Options);

		return LoadedObject;
	}
}

TArray<FString> UHeartFlakeLibrary::GetAllProviders()
{
	TArray<FString> Out;
	Algo::Transform(FHeartCoreModule::Get().GetAllProviderNames(), Out,
		[](const FName Name){ return Name.ToString(); });
	return Out;
}

FHeartFlake UHeartFlakeLibrary::CreateFlake_Struct(const FInstancedStruct& Struct, const FName Serializer)
{
	return Heart::Flakes::CreateFlake(Serializer, Struct);
}

FHeartFlake UHeartFlakeLibrary::CreateFlake(const UObject* Object, const FName Serializer)
{
	return Heart::Flakes::CreateFlake(Serializer, Object);
}

FHeartFlake_Actor UHeartFlakeLibrary::CreateFlake_Actor(const AActor* Actor, const FName Serializer)
{
	check(Actor);

	FHeartFlake_Actor Flake = Heart::Flakes::CreateFlake(Serializer, Actor);
	Flake.Transform = Actor->GetTransform();

	return Flake;
}

FInstancedStruct UHeartFlakeLibrary::ConstructStructFromFlake(const FHeartFlake& Flake,
	const UScriptStruct* ExpectedStruct, const FName Serializer)
{
	return Heart::Flakes::CreateStruct(Serializer, Flake, ExpectedStruct);
}

UObject* UHeartFlakeLibrary::ConstructObjectFromFlake(const FHeartFlake& Flake, UObject* Outer,
													  const UClass* ExpectedClass, const FName Serializer)
{
	return Heart::Flakes::CreateObject(Serializer, Flake, Outer, ExpectedClass);
}

AActor* UHeartFlakeLibrary::ConstructActorFromFlake(const FHeartFlake_Actor& Flake, UObject* WorldContextObj,
													const TSubclassOf<AActor> ExpectedClass, const FName Serializer)
{
	if (!IsValid(WorldContextObj))
	{
		return nullptr;
	}

	if (!IsValid(ExpectedClass))
	{
		return nullptr;
	}

	const TSubclassOf<AActor> ActorClass = LoadClass<AActor>(nullptr, *Flake.Struct.ToString(), nullptr, LOAD_None, nullptr);

	if (!ActorClass->IsChildOf(ExpectedClass))
	{
		return nullptr;
	}

	if (auto&& LoadedActor = WorldContextObj->GetWorld()->SpawnActorDeferred<AActor>(ActorClass, Flake.Transform))
	{
		Heart::Flakes::WriteObject(Serializer, LoadedActor, Flake);

		LoadedActor->FinishSpawning(Flake.Transform);

		return LoadedActor;
	}

	return nullptr;
}

int32 UHeartFlakeLibrary::GetNumBytes(const FHeartFlake& Flake)
{
	return Flake.Data.Num();
}