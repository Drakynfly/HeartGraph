// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "Concepts/BaseStructureProvider.h"
#include "Concepts/VariantStructureProvider.h"

#include "BloodData.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Wildcard"))
struct BLOOD_API FBloodWildcard
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, meta = (DisplayName = "Bool"))
struct BLOOD_API FBloodBool
{
	GENERATED_BODY()

	FBloodBool() {}
	FBloodBool(const bool Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	bool Value = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Byte"))
struct BLOOD_API FBloodUI8
{
	GENERATED_BODY()

	FBloodUI8() {}
	FBloodUI8(const uint8 Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	uint8 Value = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Integer"))
struct BLOOD_API FBloodI32
{
	GENERATED_BODY()

	FBloodI32() {}
	FBloodI32(const int32 Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	int32 Value = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Integer (64)"))
struct BLOOD_API FBloodI64
{
	GENERATED_BODY()

	FBloodI64() {}
	FBloodI64(const int64 Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	int64 Value = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Float"))
struct BLOOD_API FBloodFloat
{
	GENERATED_BODY()

	FBloodFloat() {}
	FBloodFloat(const float Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	float Value = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Double"))
struct BLOOD_API FBloodDouble
{
	GENERATED_BODY()

	FBloodDouble() {}
	FBloodDouble(const double Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	double Value = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Name"))
struct BLOOD_API FBloodName
{
	GENERATED_BODY()

	FBloodName() {}
	FBloodName(const FName& Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	FName Value = NAME_None;
};

USTRUCT(BlueprintType, meta = (DisplayName = "String"))
struct BLOOD_API FBloodString
{
	GENERATED_BODY()

	FBloodString() {}
	FBloodString(const FString& Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	FString Value;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Text"))
struct BLOOD_API FBloodText
{
	GENERATED_BODY()

	FBloodText() {}
	FBloodText(const FText& Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	FText Value = FText::GetEmpty();
};

USTRUCT(BlueprintType, meta = (DisplayName = "Object"))
struct BLOOD_API FBloodObject
{
	GENERATED_BODY()

	FBloodObject() {}
	FBloodObject(const TObjectPtr<UObject> Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> Value = nullptr;

	template <typename T> TObjectPtr<T> Get() const
	{
		return Cast<T>(Value);
	}
};

USTRUCT(BlueprintType, meta = (DisplayName = "Class"))
struct BLOOD_API FBloodClass
{
	GENERATED_BODY()

	FBloodClass() {}
	FBloodClass(const UClass* Value) : Value(Value) {}
	FBloodClass(const TSubclassOf<UObject> Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<const UClass> Value = nullptr;

	template <typename T> TSubclassOf<T> Get() const
	{
		return Value;
	}
};

USTRUCT(BlueprintType, meta = (DisplayName = "Object (Soft)"))
struct BLOOD_API FBloodSoftObject
{
	GENERATED_BODY()

	FBloodSoftObject() {}
	FBloodSoftObject(const TSoftObjectPtr<UObject>& Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	TSoftObjectPtr<UObject> Value = nullptr;

	template <typename T> TSoftObjectPtr<T> Get() const
	{
		return Value;
	}
};

USTRUCT(BlueprintType, meta = (DisplayName = "Class (Soft)"))
struct BLOOD_API FBloodSoftClass
{
	GENERATED_BODY()

	FBloodSoftClass() {}
	FBloodSoftClass(const TSoftClassPtr<UObject>& Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	TSoftClassPtr<UObject> Value = nullptr;

	template <typename T> TSoftClassPtr<T> Get() const
	{
		return Value;
	}
};

namespace Blood
{
	template <typename T> struct TIsPoDWrapperStruct
	{
		static constexpr bool Value = false;
	};

	template <> struct TIsPoDWrapperStruct<FBloodWildcard>	{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodBool>		{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodUI8>		{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodI32>		{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodI64>		{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodFloat>		{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodDouble>	{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodName>		{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodString>	{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodText>		{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodObject>	{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodClass>		{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodSoftObject>{ static constexpr bool Value = true; };
	template <> struct TIsPoDWrapperStruct<FBloodSoftClass>	{ static constexpr bool Value = true; };

	template <typename T> struct TDataConverter
	{
		static UScriptStruct* Type()
		{
			if constexpr (TModels_V<CBaseStructureProvider, T>)
			{
				return TBaseStructure<T>::Get();
			}

			if constexpr (TModels_V<CVariantStructureProvider, T>)
			{
				return TVariantStructure<T>::Get();
			}

			return FBloodWildcard::StaticStruct();
		}

		static T Value(const FInstancedStruct& Value)
		{
			return Value.Get<T>();
		}
	};

	// Custom Void Interpreter
	template<> struct TDataConverter<void>
	{
		static UScriptStruct* Type() { return FBloodWildcard::StaticStruct(); }
	};

	// Macro for binding a plain data type to its Blood wrapper struct
	#define BIND_BLOOD_TYPE(type, StructType)\
		template<> struct TDataConverter<type>\
		{\
			using BloodType = StructType;\
			static UScriptStruct* Type() { return BloodType::StaticStruct(); }\
			\
			static type Value(const FInstancedStruct& Value)\
			{\
				if (ensure(Value.GetScriptStruct() == Type()))\
				{\
					return Value.Get<BloodType>().Value;\
				}\
				return BloodType().Value;\
			}\
		};

	// Macro for binding a templated data type to its Blood wrapper struct
	#define BIND_BLOOD_TYPE_TEMPLATED(Wrapper, StructType)\
		template <typename T> struct TDataConverter<Wrapper<T>>\
		{\
			using BloodType = StructType;\
			static UScriptStruct* Type() { return BloodType::StaticStruct(); }\
			\
			static Wrapper<T> Value(const FInstancedStruct& Value)\
			{\
				if (ensure(Value.GetScriptStruct() == Type()))\
				{\
					return Value.Get<BloodType>().Get<T>();\
				}\
				return BloodType().Get<T>();\
			}\
		};

	BIND_BLOOD_TYPE(bool, FBloodBool)
	BIND_BLOOD_TYPE(uint8, FBloodUI8)
	BIND_BLOOD_TYPE(int32, FBloodI32)
	BIND_BLOOD_TYPE(int64, FBloodI64)
	BIND_BLOOD_TYPE(float, FBloodFloat)
	BIND_BLOOD_TYPE(double, FBloodDouble)
	BIND_BLOOD_TYPE(FName, FBloodName)
	BIND_BLOOD_TYPE(FString, FBloodString)
	BIND_BLOOD_TYPE(FText, FBloodText)
	BIND_BLOOD_TYPE_TEMPLATED(TObjectPtr, FBloodObject)
	BIND_BLOOD_TYPE_TEMPLATED(TSubclassOf, FBloodClass)
	BIND_BLOOD_TYPE_TEMPLATED(TSoftObjectPtr, FBloodSoftObject)
	BIND_BLOOD_TYPE_TEMPLATED(TSoftClassPtr, FBloodSoftClass)

#undef BIND_BLOOD_TYPE
#undef BIND_BLOOD_TYPE_TEMPLATED
}