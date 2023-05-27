// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "BloodValue.h"

namespace Blood::Impl
{
	using FFPropertyReadFunc = TFunctionRef<FBloodValue(const FProperty* ValueProp, const uint8* ValuePtr)>;

	template <typename T>
	struct TReaderLambdaGen
	{
		static FFPropertyReadFunc Make()
		{
			return [](const FProperty* ValueProp, const uint8* ValuePtr)
				{
					FBloodValue OutValue;
					OutValue.Data.InitializeAs(TDataConverter<typename T::TCppType>::Type(), ValuePtr);
					return OutValue;
				};
		}
	};

	template<>
	struct TReaderLambdaGen<FClassProperty>
	{
		static FFPropertyReadFunc Make()
		{
			return [](const FProperty* ValueProp, const uint8* ValuePtr)
				{
					FBloodValue OutValue;
					OutValue.Data.InitializeAs(FBloodClass::StaticStruct(), ValuePtr);
					return OutValue;
				};
		}
	};

	template<>
	struct TReaderLambdaGen<FSoftClassProperty>
	{
		static FFPropertyReadFunc Make()
		{
			return [](const FProperty* ValueProp, const uint8* ValuePtr)
				{
					FBloodValue OutValue;
					OutValue.Data.InitializeAs(FBloodSoftClass::StaticStruct(), ValuePtr);
					return OutValue;
				};
		}
	};

	template<>
	struct TReaderLambdaGen<FObjectProperty>
	{
		static FFPropertyReadFunc Make()
		{
			return [](const FProperty* ValueProp, const uint8* ValuePtr)
				{
					FBloodValue OutValue;
					OutValue.Data.InitializeAs(FBloodObject::StaticStruct(), ValuePtr);
					return OutValue;
				};
		}
	};

	template<>
	struct TReaderLambdaGen<FSoftObjectProperty>
	{
		static FFPropertyReadFunc Make()
		{
			return [](const FProperty* ValueProp, const uint8* ValuePtr)
				{
					FBloodValue OutValue;
					OutValue.Data.InitializeAs(FBloodSoftObject::StaticStruct(), ValuePtr);
					return OutValue;
				};
		}
	};

	template<>
	struct TReaderLambdaGen<FStructProperty>
	{
		static FFPropertyReadFunc Make()
		{
			return [](const FProperty* ValueProp, const uint8* ValuePtr)
				{
					FBloodValue OutValue;
					if (const FStructProperty* StructProp = CastField<FStructProperty>(ValueProp))
					{
						OutValue.Data.InitializeAs(StructProp->Struct, ValuePtr);
					}
					return OutValue;
				};
		}
	};

	struct FStaticFieldMaps
	{
#define DEF_READER_LAMBDA(TypeName) { F##TypeName##Property::StaticClass(), TReaderLambdaGen<F##TypeName##Property>::Make() },
		TMap<const FFieldClass*, FFPropertyReadFunc> ReaderMap = {
			DEF_READER_LAMBDA(Bool)
			DEF_READER_LAMBDA(Byte)
			DEF_READER_LAMBDA(Int)
			DEF_READER_LAMBDA(Int64)
			DEF_READER_LAMBDA(Float)
			DEF_READER_LAMBDA(Double)
			DEF_READER_LAMBDA(Name)
			DEF_READER_LAMBDA(Str)
			DEF_READER_LAMBDA(Text)
			DEF_READER_LAMBDA(Class)
			DEF_READER_LAMBDA(SoftClass)
			DEF_READER_LAMBDA(Object)
			DEF_READER_LAMBDA(SoftObject)
			DEF_READER_LAMBDA(Struct)
		};
#undef DEF_READER_LAMBDA
	};

	static FStaticFieldMaps StaticFieldMaps;

	TObjectPtr<UField> FPropertyHelpers::GetFPropertyFieldTypeImpl(const FProperty* Prop)
	{
		#define TRY_BIND_VALUE(type, actual)\
		if (auto&& type##Prop = CastField<F##type##Property>(Prop))\
		{\
			return TDataConverter<actual>::Type();\
		}

		#define TRY_BIND_VALUE_CLASS(type, InnerProp)\
		if (auto&& type##Prop = CastField<F##type##Property>(Prop))\
		{\
			return type##Prop->InnerProp;\
		}

		TRY_BIND_VALUE(Bool, bool)
		TRY_BIND_VALUE(Byte, uint8)
		TRY_BIND_VALUE(Int, int32)
		TRY_BIND_VALUE(Int64, int64)
		TRY_BIND_VALUE(Float, float)
		TRY_BIND_VALUE(Double, double)
		TRY_BIND_VALUE(Name, FName)
		TRY_BIND_VALUE(Str, FString)
		TRY_BIND_VALUE(Text, FText)
		TRY_BIND_VALUE_CLASS(Class, MetaClass)
		TRY_BIND_VALUE_CLASS(SoftClass, MetaClass)
		TRY_BIND_VALUE_CLASS(Object, PropertyClass)
		TRY_BIND_VALUE_CLASS(SoftObject, PropertyClass)
		TRY_BIND_VALUE_CLASS(Struct, Struct)

	#undef TRY_BIND_VALUE
	#undef TRY_BIND_VALUE_CLASS
		return FBloodWildcard::StaticStruct();
	}

	bool FPropertyHelpers::WriteToFPropertyValuePtr(const FProperty* ValueProp, uint8* ValuePtr, const FBloodValue& Value)
	{
		check(ValueProp);
		check(ValuePtr);

		if (auto&& ArrayProp = CastField<FArrayProperty>(ValueProp))
		{
			UE_LOG(LogTemp, Error, TEXT("Array types are not supported yet."))
			return false;
		}
		if (auto&& SetProp = CastField<FSetProperty>(ValueProp))
		{
			UE_LOG(LogTemp, Error, TEXT("Set types are not supported yet."))
			return false;
		}
		if (auto&& MapProp = CastField<FMapProperty>(ValueProp))
		{
			UE_LOG(LogTemp, Error, TEXT("Map types are not supported yet."))
			return false;
		}

		// Macro for binding to most FProperty types
	#define TRY_BIND_TYPE(TypeName)\
		if (auto&& TypeName##Prop = CastField<F##TypeName##Property>(ValueProp))\
		{\
			if (Value.Data.GetScriptStruct() == TDataConverter<F##TypeName##Property::TCppType>::Type())\
			{\
				TypeName##Prop->SetPropertyValue(ValuePtr, Value.GetValue<F##TypeName##Property::TCppType>());\
				return true;\
			}\
			return false;\
		}

		// Macro for binding to Class FProperty types
	#define TRY_BIND_TYPE_CLASS(TypeName, Transformation)\
		if (auto&& TypeName##Prop = CastField<F##TypeName##Property>(ValueProp))\
		{\
			if (Value.Data.GetScriptStruct() == FBlood##TypeName::StaticStruct())\
			{\
				TypeName##Prop->SetPropertyValue(ValuePtr, Transformation);\
				return true;\
			}\
			return false;\
		}

		TRY_BIND_TYPE(Bool)
		TRY_BIND_TYPE(Byte)
		TRY_BIND_TYPE(Float)
		TRY_BIND_TYPE(Double)
		TRY_BIND_TYPE(Int)
		TRY_BIND_TYPE(Int64)
		TRY_BIND_TYPE(Name)
		TRY_BIND_TYPE(Str)
		TRY_BIND_TYPE(Text)

		TRY_BIND_TYPE_CLASS(SoftClass, FSoftObjectPtr(Value.GetValue<TSoftClassPtr<>>().ToSoftObjectPath()));
		TRY_BIND_TYPE_CLASS(Class, Value.GetValue<FClassProperty::TCppType>());
		TRY_BIND_TYPE_CLASS(SoftObject, FSoftObjectPtr(Value.GetValue<TSoftObjectPtr<>>().ToSoftObjectPath()));
		TRY_BIND_TYPE_CLASS(Object, Value.GetValue<TObjectPtr<UObject>>());

		// Special handling for the unique way structs are initialized
		if (auto&& StructProp = CastField<FStructProperty>(ValueProp))
		{
			if (Value.Data.GetScriptStruct() == StructProp->Struct)
			{
				StructProp->CopyValuesInternal(ValuePtr, Value.Data.GetMemory(), 1);
				return true;
			}
			return false;
		}

	#undef TRY_BIND_TYPE
	#undef TRY_BIND_TYPE_CLASS

		return false;
	}

	FBloodValue FPropertyHelpers::ReadFromFPropertyValuePtr(const FProperty* ValueProp, const uint8* ValuePtr)
	{
		check(ValueProp);
		check(ValuePtr);

		const FFieldClass* FieldClass = ValueProp->GetClass();
		if (const FFPropertyReadFunc* StaticLambda = StaticFieldMaps.ReaderMap.Find(FieldClass))
		{
			return (*StaticLambda)(ValueProp, ValuePtr);
		}

		return FBloodValue();
	}
}
