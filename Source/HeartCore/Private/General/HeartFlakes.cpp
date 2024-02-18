// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "General/HeartFlakes.h"
#include "General/HeartFlakeInterface.h"

#include "Compression/OodleDataCompressionUtil.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartFlakes)

static const FName SkippedObjectMarker("_SKIP_");

FArchive& FHeartMemoryWriter::operator<<(UObject*& Obj)
{
	if (!IsValid(Obj))
	{
		return *this;
	}

	// Only serialize UObjects if we are also serializing its outer.
	const bool SkipObject = !OuterStack.Contains(Obj->GetOuter());

	FString ClassOrSkip;

	if (SkipObject)
	{
		ClassOrSkip = SkippedObjectMarker.ToString();
	}
	else
	{
		ClassOrSkip = FSoftClassPath(Obj->GetClass()).ToString();
	}

	*this << ClassOrSkip;

	if (!SkipObject)
	{
		OuterStack.Push(Obj); // Track that we are serializing this object
		if (Obj->Implements<UHeartFlakeInterface>())
		{
			IHeartFlakeInterface::Execute_PreWrite(Obj);
		}
		Obj->Serialize(*this);
		OuterStack.Pop(); // Untrack the object
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
	if (UObject* ObjPtr = AssetPtr.Get())
	{
		*this << ObjPtr;
	}
	return *this;
}

FArchive& FHeartMemoryWriter::operator<<(FWeakObjectPtr& Value)
{
	if (UObject* ObjPtr = Value.Get())
	{
		*this << ObjPtr;
	}
	return *this;
}

FString FHeartMemoryWriter::GetArchiveName() const
{
	return TEXT("FHeartMemoryWriter");
}

FArchive& FHeartMemoryReader::operator<<(UObject*& Obj)
{
	FString ClassOrSkip;
	*this << ClassOrSkip;

	if (ClassOrSkip.IsEmpty())
	{
		return *this;
	}

	if (*ClassOrSkip == SkippedObjectMarker)
	{
		return *this;
	}

	const UClass* ObjClass = FSoftClassPath(ClassOrSkip).TryLoadClass<UObject>();

	if (IsValid(ObjClass))
	{
		Obj = NewObject<UObject>(OuterStack.Last(), ObjClass);
		OuterStack.Push(Obj);
		Obj->Serialize(*this);
		if (Obj->Implements<UHeartFlakeInterface>())
		{
			IHeartFlakeInterface::Execute_PostRead(Obj);
		}
		OuterStack.Pop();
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
	UObject* RawObj = nullptr;
	*this << RawObj;
	AssetPtr = RawObj;
	return *this;
}

FArchive& FHeartMemoryReader::operator<<(FWeakObjectPtr& Value)
{
	if (UObject* ObjPtr = Value.Get())
	{
		*this << ObjPtr;
		UE_LOG(LogTemp, Warning, TEXT("FHeartMemoryReader << for <FWeakObjectPtr>: %s"), *ObjPtr->GetName());
	}
	return *this;
}

FString FHeartMemoryReader::GetArchiveName() const
{
	return TEXT("FHeartMemoryReader");
}

namespace Heart::Flakes
{
	FHeartFlake CreateFlake(const FInstancedStruct& Struct)
	{
		FHeartFlake Flake;
		Flake.Struct = Struct.GetScriptStruct()->GetPathName();

		TArray<uint8> Raw;
		FHeartMemoryWriter MemoryWriter(Raw, nullptr);
		const_cast<FInstancedStruct&>(Struct).Serialize(MemoryWriter);

		FOodleCompressedArray::CompressTArray(
			Flake.Data, Raw,
			FOodleDataCompression::ECompressor::Kraken, // @todo expose as option
			FOodleDataCompression::ECompressionLevel::SuperFast // @todo expose as option
			);

		MemoryWriter.FlushCache();
		MemoryWriter.Close();

		return Flake;
	}

	FHeartFlake CreateFlake(UObject* Object)
	{
		FHeartFlake Flake;
		Flake.Struct = Object->GetClass()->GetPathName();

		TArray<uint8> Raw;
		FHeartMemoryWriter MemoryWriter(Raw, Object);
		Object->Serialize(MemoryWriter);

		FOodleCompressedArray::CompressTArray(
			Flake.Data, Raw,
			FOodleDataCompression::ECompressor::Kraken, // @todo expose as option
			FOodleDataCompression::ECompressionLevel::SuperFast // @todo expose as option
			);

		MemoryWriter.FlushCache();
		MemoryWriter.Close();

		return Flake;
	}

	void WriteObject(UObject* Object, const FHeartFlake& Flake)
	{
		TArray<uint8> Raw;

		FOodleCompressedArray::DecompressToTArray(Raw, Flake.Data);

		FHeartMemoryReader MemoryReader(Raw, true, Object);
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