// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"

#include "BloodData.generated.h"

USTRUCT()
struct FBloodDataBase
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, meta = (DisplayName = "Wildcard"))
struct BLOOD_API FBloodWildcard : public FBloodDataBase
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, meta = (DisplayName = "Bool"))
struct BLOOD_API FBloodBool : public FBloodDataBase
{
	GENERATED_BODY()

	FBloodBool() {}
	FBloodBool(const bool Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	bool Value = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Byte"))
struct BLOOD_API FBloodUI8 : public FBloodDataBase
{
	GENERATED_BODY()

	FBloodUI8() {}
	FBloodUI8(const uint8 Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	uint8 Value = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Integer"))
struct BLOOD_API FBloodI32 : public FBloodDataBase
{
	GENERATED_BODY()

	FBloodI32() {}
	FBloodI32(const int32 Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	int32 Value = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Integer (64)"))
struct BLOOD_API FBloodI64 : public FBloodDataBase
{
	GENERATED_BODY()

	FBloodI64() {}
	FBloodI64(const int64 Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	int64 Value = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Float"))
struct BLOOD_API FBloodFloat : public FBloodDataBase
{
	GENERATED_BODY()

	FBloodFloat() {}
	FBloodFloat(const float Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	float Value = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Double"))
struct BLOOD_API FBloodDouble : public FBloodDataBase
{
	GENERATED_BODY()

	FBloodDouble() {}
	FBloodDouble(const double Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	double Value = false;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Name"))
struct BLOOD_API FBloodName : public FBloodDataBase
{
	GENERATED_BODY()

	FBloodName() {}
	FBloodName(const FName& Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	FName Value = NAME_None;
};

USTRUCT(BlueprintType, meta = (DisplayName = "String"))
struct BLOOD_API FBloodString : public FBloodDataBase
{
	GENERATED_BODY()

	FBloodString() {}
	FBloodString(const FString& Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	FString Value;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Text"))
struct BLOOD_API FBloodText : public FBloodDataBase
{
	GENERATED_BODY()

	FBloodText() {}
	FBloodText(const FText& Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	FText Value = FText::GetEmpty();
};

USTRUCT(BlueprintType, meta = (DisplayName = "Object"))
struct BLOOD_API FBloodObject : public FBloodDataBase
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
struct BLOOD_API FBloodClass : public FBloodDataBase
{
	GENERATED_BODY()

	FBloodClass() {}
	FBloodClass(const TSubclassOf<UObject> Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UObject> Value = nullptr;

	template <typename T> TSubclassOf<T> Get() const
	{
		return Value.Get();
	}
};

USTRUCT(BlueprintType, meta = (DisplayName = "Object (Soft)"))
struct BLOOD_API FBloodSoftObject : public FBloodDataBase
{
	GENERATED_BODY()

	FBloodSoftObject() {}
	FBloodSoftObject(const TSoftObjectPtr<UObject> Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	TSoftObjectPtr<UObject> Value = nullptr;

	template <typename T> TSoftObjectPtr<T> Get() const
	{
		return Value;
	}
};

USTRUCT(BlueprintType, meta = (DisplayName = "Class (Soft)"))
struct BLOOD_API FBloodSoftClass : public FBloodDataBase
{
	GENERATED_BODY()

	FBloodSoftClass() {}
	FBloodSoftClass(const TSoftClassPtr<UObject> Value) : Value(Value) {}

	UPROPERTY(BlueprintReadWrite)
	TSoftClassPtr<UObject> Value = nullptr;

	template <typename T> TSoftClassPtr<T> Get() const
	{
		return Value;
	}
};

namespace Blood
{
	template <typename T> struct TDataConverter
	{
		static UScriptStruct* Type() { return TBaseStructure<T>::Get(); }

		static T Value(const FInstancedStruct& Value)
		{
			if (ensure(Value.GetScriptStruct() == Type()))
			{
				return Value.Get<T>();
			}
			return {};
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
			static UScriptStruct* Type() { return StructType::StaticStruct(); }\
			\
			static type Value(const FInstancedStruct& Value)\
			{\
				if (ensure(Value.GetScriptStruct() == Type()))\
				{\
					return Value.Get<StructType>().Value;\
				}\
				return StructType().Value;\
			}\
		};

	// Macro for binding a templated data type to its Blood wrapper struct
	#define BIND_BLOOD_TYPE_TEMPLATED(Wrapper, StructType)\
		template<typename T> struct TDataConverter<Wrapper<T>>\
		{\
			static UScriptStruct* Type() { return StructType::StaticStruct(); }\
			\
			static Wrapper<T> Value(const FInstancedStruct& Value)\
			{\
				if (ensure(Value.GetScriptStruct() == Type()))\
				{\
					return Value.Get<StructType>().Get<T>();\
				}\
				return StructType().Get<T>();\
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