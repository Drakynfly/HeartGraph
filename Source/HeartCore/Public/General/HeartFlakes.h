// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"

#include "HeartFlakes.generated.h"

class HEARTCORE_API FHeartMemoryWriter : public FMemoryWriter
{
public:
	FHeartMemoryWriter(TArray<uint8>& OutBytes, UObject* Outer)
	  : FMemoryWriter(OutBytes),
		OuterStack({Outer}) {}

	using FMemoryWriter::operator<<; // For visibility of the overloads we don't override

	//~ Begin FArchive Interface
	virtual FArchive& operator<<(UObject*& Obj) override;
	virtual FArchive& operator<<(FObjectPtr& Obj) override;
	virtual FArchive& operator<<(FSoftObjectPtr& AssetPtr) override;
	virtual FArchive& operator<<(FSoftObjectPath& Value) override;
	virtual FArchive& operator<<(FWeakObjectPtr& Value) override;
	virtual FString GetArchiveName() const override;
	//~ End FArchive Interface

private:
	// Tracks what objects are currently being serialized. This allows us to only serialize UObjects that are directly
	// owned *and* stored in the first outer.
	TArray<UObject*> OuterStack;
};

class HEARTCORE_API FHeartMemoryReader : public FMemoryReader
{
public:
	enum EOptions
	{
		None = 0,
		ExecPostLoad = 1 << 0
	};

	FHeartMemoryReader(const TArray<uint8>& InBytes, bool bIsPersistent, UObject* Outer, EOptions Options)
	  : FMemoryReader(InBytes, bIsPersistent),
		OuterStack({Outer}),
		Options(Options) {}

	using FMemoryReader::operator<<; // For visibility of the overloads we don't override

	//~ Begin FArchive Interface
	virtual FArchive& operator<<(UObject*& Obj) override;
	virtual FArchive& operator<<(FObjectPtr& Obj) override;
	virtual FArchive& operator<<(FSoftObjectPtr& AssetPtr) override;
	virtual FArchive& operator<<(FSoftObjectPath& Value) override;
	virtual FArchive& operator<<(FWeakObjectPtr& Value) override;
	virtual FString GetArchiveName() const override;
	//~ End FArchive Interface

private:
	// Tracks what objects are currently being deserialized. This allows us to reconstruct objects with their original
	// outer.
	TArray<UObject*> OuterStack;

	TArray<UObject*> ObjectsCreated;

	EOptions Options = EOptions::None;
};

ENUM_CLASS_FLAGS(FHeartMemoryReader::EOptions)

USTRUCT(BlueprintType)
struct HEARTCORE_API FHeartFlake
{
	GENERATED_BODY()

	// This is either a UClass, or UStruct.
	UPROPERTY()
	FSoftObjectPath Struct;

	UPROPERTY()
	TArray<uint8> Data;

	friend FArchive& operator<<(FArchive& Ar, FHeartFlake& Flake)
	{
		Ar << Flake.Struct;
		Ar << Flake.Data;
		return Ar;
	}
};

USTRUCT()
struct HEARTCORE_API FHeartFlake_Actor : public FHeartFlake
{
	GENERATED_BODY()

	FHeartFlake_Actor() {}

	FHeartFlake_Actor(const FHeartFlake& Flake)
	  : FHeartFlake(Flake) {}

	UPROPERTY()
	FTransform Transform;

	friend FArchive& operator<<(FArchive& Ar, FHeartFlake_Actor& Flake)
	{
		Ar << Flake.Struct;
		Ar << Flake.Data;
		Ar << Flake.Transform;
		return Ar;
	}
};

namespace Heart::Flakes
{
	struct FReadOptions
	{
		FOodleDataCompression::ECompressor Compressor = FOodleDataCompression::ECompressor::Kraken;
		FOodleDataCompression::ECompressionLevel CompressionLevel = FOodleDataCompression::ECompressionLevel::SuperFast;
	};

	struct FWriteOptions
	{
		uint8 ExecPostLoad : 1;
	};

	HEARTCORE_API FHeartFlake CreateFlake(const FInstancedStruct& Struct, FReadOptions Options = {});

	HEARTCORE_API FHeartFlake CreateFlake(UObject* Object, FReadOptions Options = {});

	HEARTCORE_API void WriteObject(UObject* Object, const FHeartFlake& Flake, FWriteOptions Options = {});

	HEARTCORE_API UObject* CreateObject(const FHeartFlake& Flake, UObject* Outer, const UClass* ExpectedClass);

	template <typename T>
	T* CreateObject(const FHeartFlake& Flake, UObject* Outer = GetTransientPackage())
	{
		return Cast<T>(CreateObject(Flake, Outer, T::StaticClass()));
	}
}


/**
 * A simple set of functions to convert arbitrary objects/actors into "Flakes", and back. This is Heart's serialization
 * system. Flakes can be stored anywhere, written into a save file, etc, and reconstructed back into a copy of the
 * original at any time. Note that a Flake always creates a copy, it doesn't write back into the original object.
 */
UCLASS()
class HEARTCORE_API UHeartFlakeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Serialize an struct (and all its subobjects) into a flake. */
	UFUNCTION(BlueprintPure, Category = "Heart|FlakeLibrary", meta = (DisplayName = "Create Flake (Struct)"))
	static FHeartFlake CreateFlake_Struct(const FInstancedStruct& Struct);

	/** Serialize an object (and all its subobjects) into a flake. */
	UFUNCTION(BlueprintPure, Category = "Heart|FlakeLibrary", meta = (DisplayName = "Create Flake (Object)"))
	static FHeartFlake CreateFlake(UObject* Object);

	/** Serialize an actor (and all its subobjects) into a flake. */
	UFUNCTION(BlueprintPure, Category = "Heart|FlakeLibrary", meta = (DisplayName = "Create Flake (Actor)"))
	static FHeartFlake_Actor CreateFlake_Actor(AActor* Actor);

	/**
	 * Attempt to read a flake back into an object.
	 * Does not support actors! Use ConstructActorFromFlake for that instead.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|FlakeLibrary", meta = (DeterminesOutputType = "ExpectedClass"))
	static UObject* ConstructObjectFromFlake(const FHeartFlake& Flake, UObject* Outer, const UClass* ExpectedClass);

	/** Attempt to read a flake back into an actor. */
	UFUNCTION(BlueprintCallable, Category = "Heart|FlakeLibrary", meta = (WorldContext = "WorldContextObj", DeterminesOutputType = "ExpectedClass"))
	static AActor* ConstructActorFromFlake(const FHeartFlake_Actor& Flake, UObject* WorldContextObj, const TSubclassOf<AActor> ExpectedClass);
};