// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "PropertyBag.h"
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
	namespace Private
	{
		static const FLazyName V0("v_0"); // Value index 0
		static const FLazyName V1("v_1"); // Value index 1
	}

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

	template <typename T>
	struct TDataConverter
	{
		FORCEINLINE static auto Type()
		{
			if constexpr (TIsUEnumClass<T>::Value)
			{
				return StaticEnum<T>();
			}
			else if constexpr (TModels_V<CBaseStructureProvider, T>)
			{
				return TBaseStructure<T>::Get();
			}
			else if constexpr (TModels_V<CVariantStructureProvider, T>)
			{
				return TVariantStructure<T>::Get();
			}
			else return FBloodWildcard::StaticStruct();
		}

		FORCEINLINE static EPropertyBagPropertyType PropertyBagType()
		{
			if constexpr (TIsUEnumClass<T>::Value)
			{
				return EPropertyBagPropertyType::Enum;
			}
			else
			{
				return EPropertyBagPropertyType::Struct;
			}
		}

		FORCEINLINE static UObject* PropertyBagTypeObject()
		{
			return Type();
		}

		static T Read(const FInstancedPropertyBag& PropertyBag, const FName Name)
		{
			if constexpr (TIsUEnumClass<T>::Value)
			{
				return PropertyBag.GetValueEnum<T>(Name).GetValue();
			}
			else
			{
				return *reinterpret_cast<T*>(PropertyBag.GetValueStruct(Name).GetValue().GetMemory());
			}
		}

		static T ReadIndex(const FPropertyBagArrayRef& Array, const int32 Index)
		{
			if constexpr (TIsUEnumClass<T>::Value)
			{
				return Array.GetValueEnum<T>(Index).GetValue();
			}
			else
			{
				return *reinterpret_cast<const T*>(Array.GetValueStruct(Index).GetValue().GetMemory());
			}
		}

		static void Write(FInstancedPropertyBag& PropertyBag, const FName Name, const T& Value)
		{
			if constexpr (TIsUEnumClass<T>::Value)
			{
				PropertyBag.SetValueEnum<T>(Name, Value);
			}
			else
			{
				PropertyBag.SetValueStruct(Name, FConstStructView(Type(), reinterpret_cast<const uint8*>(&Value)));
			}
		}

		static void WriteIndex(FPropertyBagArrayRef& Array, const int32 Index, const T& Value)
		{
			if constexpr (TIsUEnumClass<T>::Value)
			{
				Array.SetValueEnum<T>(Index, Value);
			}
			else
			{
				Array.SetValueStruct(Index, FConstStructView(Type(), reinterpret_cast<const uint8*>(&Value)));
			}
		}
	};

	// Custom Void Interpreter
	template<> struct TDataConverter<void>
	{
		static UScriptStruct* Type() { return FBloodWildcard::StaticStruct(); }
	};

	// Macro for binding a plain data type to its Blood wrapper struct
	#define BIND_BLOOD_TYPE(type, StructType, TypeInBag, Getter)\
		template<> struct TDataConverter<type>\
		{\
			using BloodType = StructType;\
			using ActualType = type;\
			FORCEINLINE static UScriptStruct* Type() { return BloodType::StaticStruct(); }\
			FORCEINLINE static EPropertyBagPropertyType PropertyBagType()\
			{\
				return EPropertyBagPropertyType::TypeInBag;\
			}\
			\
			FORCEINLINE static UObject* PropertyBagTypeObject()\
			{\
				return nullptr;\
			}\
			\
			static ActualType Read(const FInstancedPropertyBag& PropertyBag, const FName Name)\
			{\
				return ActualType(PropertyBag.GetValue##TypeInBag(Name).GetValue());\
			}\
			\
			static void Write(FInstancedPropertyBag& PropertyBag, const FName Name, const ActualType Value)\
			{\
				PropertyBag.SetValue##TypeInBag(Name, Getter);\
			}\
			\
			static ActualType ReadIndex(const FPropertyBagArrayRef& Array, const int32 Index)\
			{\
				return ActualType(Array.GetValue##TypeInBag(Index).GetValue());\
			}\
			\
			static void WriteIndex(FPropertyBagArrayRef& Array, const int32 Index, const ActualType Value)\
			{\
				Array.SetValue##TypeInBag(Index, Getter);\
			}\
		};

	// Macro for binding a templated data type to its Blood wrapper struct
	#define BIND_BLOOD_TYPE_TEMPLATED(Wrapper, StructType, TypeInBag)\
		template <typename T> struct TDataConverter<Wrapper<T>>\
		{\
			using BloodType = StructType;\
			using ActualType = Wrapper<T>;\
			FORCEINLINE static UScriptStruct* Type() { return BloodType::StaticStruct(); }\
			FORCEINLINE static EPropertyBagPropertyType PropertyBagType()\
			{\
				return EPropertyBagPropertyType::TypeInBag;\
			}\
			\
			FORCEINLINE static UObject* PropertyBagTypeObject()\
			{\
				return T::StaticClass();\
			}\
			\
			static ActualType Read(const FInstancedPropertyBag& PropertyBag, const FName Name)\
			{\
				return ActualType(PropertyBag.GetValue##TypeInBag(Name).GetValue());\
			}\
			\
			static void Write(FInstancedPropertyBag& PropertyBag, const FName Name, const ActualType& Value)\
			{\
				PropertyBag.SetValue##TypeInBag(Name, Value.Get());\
			}\
			\
			static ActualType ReadIndex(const FPropertyBagArrayRef& Array, const int32 Index)\
			{\
				return ActualType(Array.GetValue##TypeInBag(Index).GetValue());\
			}\
			\
			static void WriteIndex(FPropertyBagArrayRef& Array, const int32 Index, const ActualType& Value)\
			{\
				Array.SetValue##TypeInBag(Index, Value.Get());\
			}\
		};


	BIND_BLOOD_TYPE(bool, FBloodBool, Bool, Value)
	BIND_BLOOD_TYPE(uint8, FBloodUI8, Byte, Value)
	BIND_BLOOD_TYPE(int32, FBloodI32, Int32, Value)
	BIND_BLOOD_TYPE(int64, FBloodI64, Int64, Value)
	BIND_BLOOD_TYPE(float, FBloodFloat, Float, Value)
	BIND_BLOOD_TYPE(double, FBloodDouble, Double, Value)
	BIND_BLOOD_TYPE(FName, FBloodName, Name, Value)
	BIND_BLOOD_TYPE(FString, FBloodString, String, Value)
	BIND_BLOOD_TYPE(FText, FBloodText, Text, Value)
	BIND_BLOOD_TYPE(FSoftObjectPtr, FBloodSoftObject, Object, Value.Get())
	BIND_BLOOD_TYPE_TEMPLATED(TObjectPtr, FBloodObject, Object)
	BIND_BLOOD_TYPE_TEMPLATED(TSubclassOf, FBloodClass, Class)
	BIND_BLOOD_TYPE_TEMPLATED(TSoftObjectPtr, FBloodSoftObject, Object)
	BIND_BLOOD_TYPE_TEMPLATED(TSoftClassPtr, FBloodSoftClass, Class)

#undef BIND_BLOOD_TYPE
#undef BIND_BLOOD_TYPE_TEMPLATED

	namespace Read
	{
		template <typename TType>
		static auto Value(const FInstancedPropertyBag& Bag, const FName Name)
		{
			if constexpr (TIsPoDWrapperStruct<TType>::Value)
			{
				return TDataConverter<decltype(TType::Value)>::Read(Bag, Name);
			}
			else
			{
				return TDataConverter<TType>::Read(Bag, Name);
			}
		}

		template <class TContainer>
		void Container1(const FInstancedPropertyBag& Bag, const FName Name, TContainer& Out)
		{
			auto MaybeArrayRef = Bag.GetArrayRef(Name);
			check(MaybeArrayRef.HasValue());

			const FPropertyBagArrayRef& ArrayRef = MaybeArrayRef.GetValue();

			for (int32 i = 0; i < ArrayRef.Num(); ++i)
			{
				Out.Add(TDataConverter<typename TContainer::ElementType>::ReadIndex(ArrayRef, i));
			}
		}

		template <class TContainer>
		void Container2(const FInstancedPropertyBag& Bag, const TPair<FName, FName>& Names, TContainer& Out)
		{
			auto MaybeArrayRef0 = Bag.GetArrayRef(Names.Key);
			auto MaybeArrayRef1 = Bag.GetArrayRef(Names.Value);
			check(MaybeArrayRef0.HasValue());
			check(MaybeArrayRef1.HasValue());

			const FPropertyBagArrayRef& ArrayRef0 = MaybeArrayRef0.GetValue();
			const FPropertyBagArrayRef& ArrayRef1 = MaybeArrayRef1.GetValue();

			for (int32 i = 0; i < ArrayRef0.Num(); ++i)
			{
				Out.Add(TDataConverter<typename TContainer::KeyType>::ReadIndex(ArrayRef0, i),
						TDataConverter<typename TContainer::ValueType>::ReadIndex(ArrayRef1, i));
			}
		}
	}

	namespace Write
	{
		template <typename TType>
		static void Value(FInstancedPropertyBag& Bag, FName Name, const TType& Value)
		{
			// Init property
			Bag.AddProperty(
				Name,
				TDataConverter<TType>::PropertyBagType(),
				TDataConverter<TType>::PropertyBagTypeObject());

			if constexpr (TIsPoDWrapperStruct<TType>::Value)
			{
				TDataConverter<TType>::Write(Bag, Name, Value.Value);
			}
			else
			{
				TDataConverter<TType>::Write(Bag, Name, Value);
			}
		}

		template <class TContainer>
		void Container1(FInstancedPropertyBag& Bag, FName Name, const TContainer& Value)
		{
			// Init property
			Bag.AddContainerProperty(
				Name,
				EPropertyBagContainerType::Array,
				TDataConverter<typename TContainer::ElementType>::PropertyBagType(),
				TDataConverter<typename TContainer::ElementType>::PropertyBagTypeObject());

			auto&& BetterBeArray = Bag.GetMutableArrayRef(Name);
			check(BetterBeArray.HasValue());

			FPropertyBagArrayRef& ArrayRef = BetterBeArray.GetValue();

			ArrayRef.AddValues(Value.Num());
			int32 Index = 0;
			for (auto&& Element : Value)
			{
				TDataConverter<typename TContainer::ElementType>::WriteIndex(ArrayRef, Index, Element);
				Index++;
			}
		}

		template <class TContainer>
		void Container2(FInstancedPropertyBag& Bag, const TPair<FName, FName>& Names, const TContainer& Value)
		{
			// Init properties
			const TArray<FPropertyBagPropertyDesc> PropertyDescs{
				FPropertyBagPropertyDesc(Names.Key, EPropertyBagContainerType::Array, TDataConverter<typename TContainer::KeyType>::PropertyBagType(), TDataConverter<typename TContainer::KeyType>::PropertyBagTypeObject()),
				FPropertyBagPropertyDesc(Names.Value, EPropertyBagContainerType::Array, TDataConverter<typename TContainer::ValueType>::PropertyBagType(), TDataConverter<typename TContainer::ValueType>::PropertyBagTypeObject())
			};

			Bag.AddProperties(PropertyDescs);

			auto&& BetterBeArray0 = Bag.GetMutableArrayRef(Names.Key);
			auto&& BetterBeArray1 = Bag.GetMutableArrayRef(Names.Value);
			check(BetterBeArray0.HasValue());
			check(BetterBeArray1.HasValue());

			FPropertyBagArrayRef& ArrayRef0 = BetterBeArray0.GetValue();
			FPropertyBagArrayRef& ArrayRef1 = BetterBeArray1.GetValue();

			ArrayRef0.AddValues(Value.Num());
			ArrayRef1.AddValues(Value.Num());
			int32 Index = 0;
			for (auto&& Element : Value)
			{
				TDataConverter<typename TContainer::KeyType>::WriteIndex(ArrayRef0, Index, Element.Key);
				TDataConverter<typename TContainer::ValueType>::WriteIndex(ArrayRef1, Index, Element.Value);
				Index++;
			}
		}
	}
}