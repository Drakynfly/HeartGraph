// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "General/HeartFlakes.h"

#include "General/HeartFlakeInterface.h"

static const FName SkippedObjectMarker = TEXT("_SKIP_");

FArchive& FHeartMemoryWriter::operator<<(UObject*& Obj)
{
	// Only serialize UObjects if we are also serializing its outer.
	const bool SkipObject = !OuterStack.Contains(Obj->GetOuter());

	FString ClassOrSkip;

	if (SkipObject)
	{
		ClassOrSkip = SkippedObjectMarker.ToString();
	}
	else
	{
		ClassOrSkip = Obj->GetClass()->GetPathName();
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

	if (*ClassOrSkip != SkippedObjectMarker)
	{
		if (ClassOrSkip.IsEmpty())
		{
			return *this;
		}

		const UClass* ObjClass = FindObject<UClass>(nullptr, *ClassOrSkip);
		if (!IsValid(ObjClass))
		{
			ObjClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *ClassOrSkip,
				nullptr, LOAD_None, nullptr));
		}

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

FHeartFlake UHeartFlakeLibrary::CreateFlake(UObject* Object)
{
	check(Object && !Object->IsA<AActor>());

	FHeartFlake Flake;
    Flake.Class = Object->GetClass()->GetPathName();

    FHeartMemoryWriter MemoryWriter(Flake.Data, Object);
    Object->Serialize(MemoryWriter);

    MemoryWriter.FlushCache();
    MemoryWriter.Close();

	return Flake;
}

FHeartFlake_Actor UHeartFlakeLibrary::CreateFlake_Actor(AActor* Actor)
{
	check(Actor);

	FHeartFlake_Actor Flake;
	Flake.Class = Actor->GetClass()->GetPathName();

	FHeartMemoryWriter MemoryWriter(Flake.Data, Actor);
	Actor->Serialize(MemoryWriter);

	Flake.Transform = Actor->GetTransform();

	MemoryWriter.FlushCache();
	MemoryWriter.Close();

	return Flake;
}

UObject* UHeartFlakeLibrary::ConstructObjectFromFlake(const FHeartFlake& Flake, UObject* Outer,
                                                      const UClass* ExpectedClass)
{
	if (!IsValid(ExpectedClass) || ExpectedClass->IsChildOf<AActor>())
	{
		return nullptr;
	}

	const FString ClassStr = Flake.Class;
	if (ClassStr.IsEmpty())
	{
		return nullptr;
	}

	auto&& ObjClass = FindObject<UClass>(nullptr, *ClassStr);
	if (!IsValid(ObjClass))
	{
		ObjClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *ClassStr, nullptr, LOAD_None, nullptr));
	}

	if (!IsValid(ObjClass))
	{
		return nullptr;
	}

	if (!ObjClass->IsChildOf(ExpectedClass))
	{
		return nullptr;
	}

	auto&& LoadedObject = NewObject<UObject>(Outer, ObjClass);

	FHeartMemoryReader MemoryReader(Flake.Data, true, LoadedObject);
	LoadedObject->Serialize(MemoryReader);

	MemoryReader.FlushCache();
	MemoryReader.Close();

	return LoadedObject;
}

AActor* UHeartFlakeLibrary::ConstructActorFromFlake(const FHeartFlake_Actor& Flake, UObject* WorldContextObj,
	const TSubclassOf<AActor> ExpectedClass)
{
	if (!IsValid(ExpectedClass) || ExpectedClass->IsChildOf<AActor>())
	{
		return nullptr;
	}

	if (!IsValid(WorldContextObj))
	{
		return nullptr;
	}

	const FString ClassStr = Flake.Class;
	if (ClassStr.IsEmpty())
	{
		return nullptr;
	}

	auto&& ActorClass = FindObject<UClass>(nullptr, *ClassStr);
	if (!IsValid(ActorClass))
	{
		ActorClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *ClassStr, nullptr, LOAD_None, nullptr));
	}

	if (!IsValid(ActorClass))
	{
		return nullptr;
	}

	if (!ActorClass->IsChildOf(ExpectedClass))
	{
		return nullptr;
	}

	if (auto&& LoadedActor = WorldContextObj->GetWorld()->SpawnActorDeferred<AActor>(ActorClass, FTransform::Identity))
	{
		LoadedActor->FinishSpawning(Flake.Transform);

		FHeartMemoryReader MemoryReader(Flake.Data, true, LoadedActor);
		LoadedActor->Serialize(MemoryReader);

		MemoryReader.FlushCache();
		MemoryReader.Close();

		return LoadedActor;
	}

	return nullptr;
}