// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "BloodFProperty.h"
#include "BloodValue.h"
#include "BloodPrecomputedMaps.h"

namespace Blood::Impl
{
	TObjectPtr<UField> FPropertyHelpers::GetFPropertyFieldType(const FProperty* Prop)
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
		TRY_BIND_VALUE_CLASS(Enum, GetEnum())
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
			unimplemented()
			return false;
		}
		if (auto&& SetProp = CastField<FSetProperty>(ValueProp))
		{
			UE_LOG(LogTemp, Error, TEXT("Set types are not supported yet."))
			unimplemented()
			return false;
		}
		if (auto&& MapProp = CastField<FMapProperty>(ValueProp))
		{
			UE_LOG(LogTemp, Error, TEXT("Map types are not supported yet."))
			unimplemented()
			return false;
		}

		// Macro for binding to most FProperty types
	#define TRY_BIND_TYPE(TypeName)\
		if (auto&& TypeName##Prop = CastField<F##TypeName##Property>(ValueProp))\
		{\
			if (Value.Is<F##TypeName##Property::TCppType>())\
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
			if (Value.Is<TDataConverter<F##TypeName##Property::TCppType>::BloodType>())\
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

		// Special handling for the unique way enums are initialized
		if (auto&& EnumProp = CastField<FEnumProperty>(ValueProp))
		{
			if (Value.Is(EnumProp->GetEnum()))
			{
				EnumProp->CopyCompleteValue(ValuePtr, Value.GetMemory());
				return true;
			}
			return false;
		}

		// Special handling for the unique way structs are initialized
		if (auto&& StructProp = CastField<FStructProperty>(ValueProp))
		{
			if (Value.Is(StructProp->Struct))
			{
				StructProp->CopyValuesInternal(ValuePtr, Value.GetMemory(), 1);
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

		if (auto&& ArrayProp = CastField<FArrayProperty>(ValueProp))
		{
			UE_LOG(LogTemp, Error, TEXT("Array types are not supported yet."))
			unimplemented()
			return FBloodValue();
		}
		if (auto&& SetProp = CastField<FSetProperty>(ValueProp))
		{
			UE_LOG(LogTemp, Error, TEXT("Set types are not supported yet."))
			unimplemented()
			return FBloodValue();
		}
		if (auto&& MapProp = CastField<FMapProperty>(ValueProp))
		{
			UE_LOG(LogTemp, Error, TEXT("Map types are not supported yet."))
			unimplemented()
			return FBloodValue();
		}

		const FFieldClass* FieldClass = ValueProp->GetClass();
		if (const FFPropertyReadFunc* StaticLambda = FPrecomputedMaps::Get().ReaderMap.Find(FieldClass))
		{
			return (*StaticLambda)(ValueProp, ValuePtr);
		}

		return FBloodValue();
	}
}