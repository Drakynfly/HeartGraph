// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "BloodFProperty.h"
#include "BloodLog.h"
#include "BloodValue.h"

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
		}

		if (auto&& ClassProp = CastField<FClassProperty>(ValueProp))
		{
			if (Value.CanCastTo<FBloodObject>())
			{
				ClassProp->SetPropertyValue(ValuePtr, Value.GetValue<TObjectPtr<UObject>>());
				return true;
			}
			return false;
		}

		if (auto&& SoftObjectProp = CastField<FSoftObjectProperty>(ValueProp))
		{
			if (Value.CanCastTo<FBloodSoftObject>())
			{
				SoftObjectProp->SetPropertyValue(ValuePtr, FSoftObjectPtr(Value.GetValue<TSoftObjectPtr<>>().ToSoftObjectPath()));
				return true;
			}
			return false;
		}

		if (auto&& ObjectProp = CastField<FObjectProperty>(ValueProp))
		{
			if (Value.CanCastTo<FBloodObject>())
			{
				ObjectProp->SetPropertyValue(ValuePtr, Value.GetValue<TObjectPtr<UObject>>());
				return true;
			}
			return false;
		}

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

	template <typename T>
	FBloodValue ReaderFunc(const T* ValueProp, const uint8* ValuePtr)
	{
		return ToBloodValue(*reinterpret_cast<const typename T::TCppType*>(ValuePtr));
	}

	// We have to specialize this because FSoftClassProperty's TCppType is FSoftObjectPtr, which Blood doesn't recognize
	FBloodValue ReaderFuncSoftClass(const FSoftClassProperty* ValueProp, const uint8* ValuePtr)
	{
		return ToBloodValue(*reinterpret_cast<const TSoftObjectPtr<>*>(ValuePtr));
	}

	// We have to specialize this because FSoftObjectProperty's TCppType is FSoftObjectPtr, which Blood doesn't recognize
	FBloodValue ReaderFuncSoftObject(const FSoftObjectProperty* ValueProp, const uint8* ValuePtr)
	{
		return ToBloodValue(*reinterpret_cast<const TSoftObjectPtr<>*>(ValuePtr));
	}

	FBloodValue ReaderFuncEnum(const FEnumProperty* EnumProp, const uint8* ValuePtr)
	{
		return FBloodValue(EnumProp->GetEnum(), ValuePtr);
	}

	FBloodValue ReaderFuncStruct(const FStructProperty* StructProp, const uint8* ValuePtr)
	{
		return FBloodValue(StructProp->Struct, ValuePtr);
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

		if (auto&& BoolProp = CastField<FBoolProperty>(ValueProp))
		{
			return ReaderFunc<FBoolProperty>(BoolProp, ValuePtr);
		}
		if (auto&& ByteProp = CastField<FByteProperty>(ValueProp))
		{
			return ReaderFunc<FByteProperty>(ByteProp, ValuePtr);
		}
		if (auto&& IntProp = CastField<FIntProperty>(ValueProp))
		{
			return ReaderFunc<FIntProperty>(IntProp, ValuePtr);
		}
		if (auto&& Int64Prop = CastField<FInt64Property>(ValueProp))
		{
			return ReaderFunc<FInt64Property>(Int64Prop, ValuePtr);
		}
		if (auto&& FloatProp = CastField<FFloatProperty>(ValueProp))
		{
			return ReaderFunc<FFloatProperty>(FloatProp, ValuePtr);
		}
		if (auto&& DoubleProp = CastField<FDoubleProperty>(ValueProp))
		{
			return ReaderFunc<FDoubleProperty>(DoubleProp, ValuePtr);
		}
		if (auto&& NameProp = CastField<FNameProperty>(ValueProp))
		{
			return ReaderFunc<FNameProperty>(NameProp, ValuePtr);
		}
		if (auto&& StringProp = CastField<FStrProperty>(ValueProp))
		{
			return ReaderFunc<FStrProperty>(StringProp, ValuePtr);
		}
		if (auto&& TextProp = CastField<FTextProperty>(ValueProp))
		{
			return ReaderFunc<FTextProperty>(TextProp, ValuePtr);
		}
		if (auto&& ClassProp = CastField<FClassProperty>(ValueProp))
		{
			return ReaderFunc<FClassProperty>(ClassProp, ValuePtr);
		}
		if (auto&& SoftClassProp = CastField<FSoftClassProperty>(ValueProp))
		{
			return ReaderFuncSoftClass(SoftClassProp, ValuePtr);
		}
		if (auto&& ObjectProp = CastField<FObjectProperty>(ValueProp))
		{
			return ReaderFunc<FObjectProperty>(ObjectProp, ValuePtr);
		}
		if (auto&& SoftObjectProp = CastField<FSoftClassProperty>(ValueProp))
		{
			return ReaderFuncSoftObject(SoftObjectProp, ValuePtr);
		}
		if (auto&& EnumProp = CastField<FEnumProperty>(ValueProp))
		{
			return ReaderFuncEnum(EnumProp, ValuePtr);
		}
		if (auto&& StructProp = CastField<FStructProperty>(ValueProp))
		{
			return ReaderFuncStruct(StructProp, ValuePtr);
		}

		UE_LOG(LogBlood, Error, TEXT("Unsupported property type %s"), ToCStr(ValueProp->GetName()));
		return FBloodValue();
	}
}
