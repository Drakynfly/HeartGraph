// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "BloodFProperty.h"
#include "BloodLog.h"
#include "BloodValue.h"
#include "BloodPrecomputedMaps.h"

#include "UObject/TextProperty.h"

namespace Blood::Impl
{
	template <typename TPropType>
	static bool TryBindType_POD(const FProperty* ValueProp, uint8* ValuePtr, const FBloodValue& Value)
	{
		if (const TPropType* CastProp = CastField<TPropType>(ValueProp))
		{
			if (Value.CanCastTo<typename TDataConverter<typename TPropType::TCppType>::BloodType>())
			{
				CastProp->SetPropertyValue(ValuePtr, Value.GetValue<typename TPropType::TCppType>());
				return true;
			}
		}
		return false;
	};

	TObjectPtr<const UField> FPropertyHelpers::GetFPropertyFieldType(const FProperty* Prop)
	{
	#define TRY_BIND_VALUE(type)\
		if (Prop->IsA<F##type##Property>())\
		{\
			return TDataConverter<F##type##Property::TCppType>::Type();\
		}

	#define TRY_BIND_VALUE_CLASS(type, InnerProp)\
		if (auto&& type##Prop = CastField<F##type##Property>(Prop))\
		{\
			return type##Prop->InnerProp;\
		}

		TRY_BIND_VALUE(Bool)
		TRY_BIND_VALUE(Byte)
		TRY_BIND_VALUE(Int)
		TRY_BIND_VALUE(Int64)
		TRY_BIND_VALUE(Float)
		TRY_BIND_VALUE(Double)
		TRY_BIND_VALUE(Name)
		TRY_BIND_VALUE(Str)
		TRY_BIND_VALUE(Text)
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
			UE_LOG(LogBlood, Error, TEXT("Array types are not supported yet."))
			unimplemented()
			return false;
		}
		if (auto&& SetProp = CastField<FSetProperty>(ValueProp))
		{
			UE_LOG(LogBlood, Error, TEXT("Set types are not supported yet."))
			unimplemented()
			return false;
		}
		if (auto&& MapProp = CastField<FMapProperty>(ValueProp))
		{
			UE_LOG(LogBlood, Error, TEXT("Map types are not supported yet."))
			unimplemented()
			return false;
		}

		if (TryBindType_POD<FBoolProperty>(ValueProp, ValuePtr, Value)) return true;
		if (TryBindType_POD<FByteProperty>(ValueProp, ValuePtr, Value)) return true;
		if (TryBindType_POD<FFloatProperty>(ValueProp, ValuePtr, Value)) return true;
		if (TryBindType_POD<FDoubleProperty>(ValueProp, ValuePtr, Value)) return true;
		if (TryBindType_POD<FIntProperty>(ValueProp, ValuePtr, Value)) return true;
		if (TryBindType_POD<FInt64Property>(ValueProp, ValuePtr, Value)) return true;
		if (TryBindType_POD<FNameProperty>(ValueProp, ValuePtr, Value)) return true;
		if (TryBindType_POD<FStrProperty>(ValueProp, ValuePtr, Value)) return true;
		if (TryBindType_POD<FTextProperty>(ValueProp, ValuePtr, Value)) return true;

		if (auto&& SoftClassProp = CastField<FSoftClassProperty>(ValueProp))
		{
			if (Value.CanCastTo<FBloodSoftObject>())
			{
				SoftClassProp->SetPropertyValue(ValuePtr, FSoftObjectPtr(Value.GetValue<TSoftClassPtr<>>().ToSoftObjectPath()));
				return true;
			}
			return false;
		};

		if (auto&& ClassProp = CastField<FClassProperty>(ValueProp))
		{
			if (Value.CanCastTo<FBloodObject>())
			{
				ClassProp->SetPropertyValue(ValuePtr, Value.GetValue<TObjectPtr<UObject>>());
				return true;
			}
			return false;
		};

		if (auto&& SoftObjectProp = CastField<FSoftObjectProperty>(ValueProp))
		{
			if (Value.CanCastTo<FBloodSoftObject>())
			{
				SoftObjectProp->SetPropertyValue(ValuePtr, FSoftObjectPtr(Value.GetValue<TSoftObjectPtr<>>().ToSoftObjectPath()));
				return true;
			}
			return false;
		};

		if (auto&& ObjectProp = CastField<FObjectProperty>(ValueProp))
		{
			if (Value.CanCastTo<FBloodObject>())
			{
				ObjectProp->SetPropertyValue(ValuePtr, Value.GetValue<TObjectPtr<UObject>>());
				return true;
			}
			return false;
		};

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

		return false;
	}

	FBloodValue FPropertyHelpers::ReadFromFPropertyValuePtr(const FProperty* ValueProp, const uint8* ValuePtr)
	{
		check(ValueProp);
		check(ValuePtr);

		if (auto&& ArrayProp = CastField<FArrayProperty>(ValueProp))
		{
			UE_LOG(LogBlood, Error, TEXT("Array types are not supported yet."))
			unimplemented()
			return FBloodValue();
		}
		if (auto&& SetProp = CastField<FSetProperty>(ValueProp))
		{
			UE_LOG(LogBlood, Error, TEXT("Set types are not supported yet."))
			unimplemented()
			return FBloodValue();
		}
		if (auto&& MapProp = CastField<FMapProperty>(ValueProp))
		{
			UE_LOG(LogBlood, Error, TEXT("Map types are not supported yet."))
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
