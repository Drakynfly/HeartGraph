// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Concepts/BaseStructureProvider.h"
#include "StructUtils/InstancedStruct.h"
#include "StructUtils/StructView.h"

#include "HeartFlakes.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFlakes, Log, All)

namespace Heart::Flakes
{
	class HEARTCORE_API FRecursiveMemoryWriter : public FMemoryWriter
	{
	public:
		FRecursiveMemoryWriter(TArray<uint8>& OutBytes, const UObject* Outer)
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
		TArray<const UObject*> OuterStack;
	};

	class HEARTCORE_API FRecursiveMemoryReader : public FMemoryReader
	{
	public:
		FRecursiveMemoryReader(const TArray<uint8>& InBytes, bool bIsPersistent, UObject* Outer)
		  : FMemoryReader(InBytes, bIsPersistent),
			OuterStack({Outer}) {}

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
	};
}


USTRUCT(BlueprintType)
struct HEARTCORE_API FHeartFlake
{
	GENERATED_BODY()

	// This is either a UClass, or UScriptStruct.
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
		// Calls PostLoad on the outermost UObject after deserialization, or PostScriptConstruct when deserializing structs.
		uint8 ExecPostLoadOrPostScriptConstruct : 1;
	};

	namespace Private
	{
		HEARTCORE_API void CompressFlake(FHeartFlake& Flake, const TArray<uint8>& Raw,
			const FOodleDataCompression::ECompressor Compressor,
			const FOodleDataCompression::ECompressionLevel CompressionLevel);

		HEARTCORE_API void DecompressFlake(const FHeartFlake& Flake, TArray<uint8>& Raw);

		HEARTCORE_API void PostLoadStruct(const FStructView& Struct);
		HEARTCORE_API void PostLoadUObject(UObject* Object);
	}

	// Low-level binary-only flake API
	HEARTCORE_API FHeartFlake CreateFlake(const FConstStructView& Struct, const FReadOptions Options = {});
	HEARTCORE_API FHeartFlake CreateFlake(const UObject* Object, const FReadOptions Options = {});
	HEARTCORE_API void WriteStruct(const FStructView& Struct, const FHeartFlake& Flake);
	HEARTCORE_API void WriteObject(UObject* Object, const FHeartFlake& Flake);
	HEARTCORE_API FInstancedStruct CreateStruct(const FHeartFlake& Flake, const UScriptStruct* ExpectedStruct);
	HEARTCORE_API UObject* CreateObject(const FHeartFlake& Flake, UObject* Outer, const UClass* ExpectedClass);

	template <
		typename T
		UE_REQUIRES(TModels_V<CBaseStructureProvider, T>)
	>
	FHeartFlake CreateFlake(const T& Struct)
	{
		return CreateFlake(FInstancedStruct::Make(Struct));
	}

	template <
		typename T
		UE_REQUIRES(TModels_V<CBaseStructureProvider, T>)
	>
	void WriteStruct(T& Struct, const FHeartFlake& Flake)
	{
		if (Flake.Struct != TBaseStructure<T>::Get())
		{
			return;
		}

		WriteStruct(FStructView::Make(Struct), Flake);
	}

	template <typename T>
	T* CreateObject(const FHeartFlake& Flake, UObject* Outer = GetTransientPackage())
	{
		return Cast<T>(CreateObject(Flake, Outer, T::StaticClass()));
	}

	struct ISerializationProvider : FVirtualDestructor, FNoncopyable
	{
		virtual FName GetProviderName() = 0;
		virtual void ReadData(const FConstStructView& Struct, TArray<uint8>& OutData) = 0;
		virtual void ReadData(const UObject* Object, TArray<uint8>& OutData) = 0;
		virtual void WriteData(const FStructView& Struct, const TArray<uint8>& Data) = 0;
		virtual void WriteData(UObject* Object, const TArray<uint8>& Data) = 0;
	};

	template <typename Impl>
	struct TSerializationProvider : ISerializationProvider
	{
		virtual void ReadData(const FConstStructView& Struct, TArray<uint8>& OutData) override final
		{
			Impl::Static_ReadData(Struct, OutData);
		}
		virtual void ReadData(const UObject* Object, TArray<uint8>& OutData) override final
		{
			Impl::Static_ReadData(Object, OutData);
		}
		virtual void WriteData(const FStructView& Struct, const TArray<uint8>& Data) override final
		{
			Impl::Static_WriteData(Struct, Data);
		}
		virtual void WriteData(UObject* Object, const TArray<uint8>& Data) override final
		{
			Impl::Static_WriteData(Object, Data);
		}
	};

#define SERIALIZATION_PROVIDER_HEADER(Name)\
	struct FSerializationProvider_##Name final : TSerializationProvider<FSerializationProvider_##Name>\
	{\
		virtual FName GetProviderName() override;\
		static void Static_ReadData(const FConstStructView& Struct, TArray<uint8>& OutData);\
		static void Static_ReadData(const UObject* Object, TArray<uint8>& OutData);\
		static void Static_WriteData(const FStructView& Struct, const TArray<uint8>& Data);\
		static void Static_WriteData(UObject* Object, const TArray<uint8>& Data);\
	};

	SERIALIZATION_PROVIDER_HEADER(Binary)

	// Low-level non-template flake API
	HEARTCORE_API FHeartFlake CreateFlake(FName Serializer, const FConstStructView& Struct, FReadOptions Options = {});
	HEARTCORE_API FHeartFlake CreateFlake(FName Serializer, const UObject* Object, FReadOptions Options = {});
	HEARTCORE_API void WriteStruct(FName Serializer, const FStructView& Struct, const FHeartFlake& Flake, FWriteOptions Options = {});
	HEARTCORE_API void WriteObject(FName Serializer, UObject* Object, const FHeartFlake& Flake, FWriteOptions Options = {});
	HEARTCORE_API FInstancedStruct CreateStruct(FName Serializer, const FHeartFlake& Flake, const UScriptStruct* ExpectedStruct);
	HEARTCORE_API UObject* CreateObject(FName Serializer, const FHeartFlake& Flake, UObject* Outer, const UClass* ExpectedClass);

	// Low-level templated flake API
	template <
		typename TSerializationProvider
		UE_REQUIRES(TIsDerivedFrom<TSerializationProvider, ISerializationProvider>::Value)
	>
	static FHeartFlake CreateFlake(const FConstStructView& Struct, const FReadOptions Options = {})
	{
		check(Struct.IsValid())

		FHeartFlake Flake;
		Flake.Struct = Struct.GetScriptStruct();

		TArray<uint8> Raw;
		TSerializationProvider::Static_ReadData(Struct, Raw);
		Private::CompressFlake(Flake, Raw, Options.Compressor, Options.CompressionLevel);

		return Flake;
	}

	template <
		typename TSerializationProvider
		UE_REQUIRES(TIsDerivedFrom<TSerializationProvider, ISerializationProvider>::Value)
	>
	static FHeartFlake CreateFlake(const UObject* Object, const FReadOptions Options = {})
	{
		check(Object && !Object->IsA<AActor>());

		FHeartFlake Flake;
		Flake.Struct = Object->GetClass();

		TArray<uint8> Raw;
		TSerializationProvider::Static_ReadData(Object, Raw);
		Private::CompressFlake(Flake, Raw, Options.Compressor, Options.CompressionLevel);

		return Flake;
	}

	template <
		typename TSerializationProvider
		UE_REQUIRES(TIsDerivedFrom<TSerializationProvider, ISerializationProvider>::Value)
	>
	static void WriteStruct(const FStructView& Struct, const FHeartFlake& Flake, const FWriteOptions Options = {})
	{
		TArray<uint8> Raw;
		Private::DecompressFlake(Flake, Raw);

		TSerializationProvider::Static_WriteData(Struct, Raw);

		if (Options.ExecPostLoadOrPostScriptConstruct)
		{
			Private::PostLoadStruct(Struct);
		}
	}

	template <
		typename TSerializationProvider
		UE_REQUIRES(TIsDerivedFrom<TSerializationProvider, ISerializationProvider>::Value)
	>
	void WriteObject(UObject* Object, const FHeartFlake& Flake, const FWriteOptions Options = {})
	{
		TArray<uint8> Raw;
		Private::DecompressFlake(Flake, Raw);

		TSerializationProvider::Static_WriteData(Object, Raw);

		if (Options.ExecPostLoadOrPostScriptConstruct)
		{
			Private::PostLoadUObject(Object);
		}
	}

	template <
		typename TSerializationProvider
		UE_REQUIRES(TIsDerivedFrom<TSerializationProvider, ISerializationProvider>::Value)
	>
	FInstancedStruct CreateStruct(const FHeartFlake& Flake, const UScriptStruct* ExpectedStruct)
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

		WriteStruct<TSerializationProvider>(CreatedStruct, Flake, Options);

		return CreatedStruct;
	}

	template <
		typename TSerializationProvider
		UE_REQUIRES(TIsDerivedFrom<TSerializationProvider, ISerializationProvider>::Value)
	>
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

		FWriteOptions Options;
		Options.ExecPostLoadOrPostScriptConstruct = true;

		WriteObject<TSerializationProvider>(LoadedObject, Flake, Options);

		return LoadedObject;
	}

	template <
		typename T,
		typename TSerializationProvider
		UE_REQUIRES(TIsDerivedFrom<TSerializationProvider, ISerializationProvider>::Value)
	>
	T* CreateObject(const FHeartFlake& Flake, UObject* Outer = GetTransientPackage())
	{
		return Cast<T>(CreateObject<TSerializationProvider>(Flake, Outer, T::StaticClass()));
	}
}


/**
 * A simple set of functions to convert arbitrary objects/actors into "Flakes", and back. This is Heart's serialization
 * system. Flakes can be stored anywhere, written into a save file, etc., and reconstructed back into a copy of the
 * original at any time. Note that a Flake always creates a copy, it doesn't write back into the original object.
 */
UCLASS()
class HEARTCORE_API UHeartFlakeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION()
	static TArray<FString> GetAllProviders();

public:
	/** Serialize a struct into a flake. */
	UFUNCTION(BlueprintPure, Category = "Heart|FlakeLibrary", meta = (DisplayName = "Create Flake (Struct)"))
	static FHeartFlake CreateFlake_Struct(const FInstancedStruct& Struct,
		UPARAM(meta=(GetOptions="HeartCore.HeartFlakeLibrary.GetAllProviders")) FName Serializer = FName("Binary"));

	/** Serialize an object (and all its subobjects) into a flake. */
	UFUNCTION(BlueprintPure, Category = "Heart|FlakeLibrary", meta = (DisplayName = "Create Flake (Object)"))
	static FHeartFlake CreateFlake(const UObject* Object,
		UPARAM(meta=(GetOptions="HeartCore.HeartFlakeLibrary.GetAllProviders")) FName Serializer = FName("Binary"));

	/** Serialize an actor (and all its subobjects) into a flake. */
	UFUNCTION(BlueprintPure, Category = "Heart|FlakeLibrary", meta = (DisplayName = "Create Flake (Actor)"))
	static FHeartFlake_Actor CreateFlake_Actor(const AActor* Actor,
		UPARAM(meta=(GetOptions="HeartCore.HeartFlakeLibrary.GetAllProviders")) FName Serializer = FName("Binary"));

	/**
	 * Attempt to read a flake back into a struct.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|FlakeLibrary")
	static FInstancedStruct ConstructStructFromFlake(const FHeartFlake& Flake, const UScriptStruct* ExpectedStruct,
		UPARAM(meta=(GetOptions="HeartCore.HeartFlakeLibrary.GetAllProviders")) FName Serializer = FName("Binary"));

	/**
	 * Attempt to read a flake back into an object.
	 * Does not support actors! Use ConstructActorFromFlake for that instead.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|FlakeLibrary", meta = (DeterminesOutputType = "ExpectedClass"))
	static UObject* ConstructObjectFromFlake(const FHeartFlake& Flake, UObject* Outer, const UClass* ExpectedClass,
		UPARAM(meta=(GetOptions="HeartCore.HeartFlakeLibrary.GetAllProviders")) FName Serializer = FName("Binary"));

	/** Attempt to read a flake back into an actor. */
	UFUNCTION(BlueprintCallable, Category = "Heart|FlakeLibrary", meta = (WorldContext = "WorldContextObj", DeterminesOutputType = "ExpectedClass"))
	static AActor* ConstructActorFromFlake(const FHeartFlake_Actor& Flake, UObject* WorldContextObj, const TSubclassOf<AActor> ExpectedClass,
		UPARAM(meta=(GetOptions="HeartCore.HeartFlakeLibrary.GetAllProviders")) FName Serializer = FName("Binary"));

	/** Get the size of the data payload in a flake */
	UFUNCTION(BlueprintPure, Category = "Heart|FlakeLibrary")
	static int32 GetNumBytes(const FHeartFlake& Flake);
};