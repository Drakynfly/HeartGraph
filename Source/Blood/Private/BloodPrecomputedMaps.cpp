// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "BloodPrecomputedMaps.h"

#include "UObject/TextProperty.h"

namespace Blood
{
	template <typename T>
	FBloodValue ReaderFunc(const FProperty* ValueProp, const uint8* ValuePtr)
	{
		return ToBloodValue(*reinterpret_cast<const typename T::TCppType*>(ValuePtr));
	}

	// We have to specialize this because FSoftClassProperty's TCppType is FSoftObjectPtr, which Blood doesn't recognize
	FBloodValue ReaderFuncSoftClass(const FProperty* ValueProp, const uint8* ValuePtr)
	{
		return ToBloodValue(*reinterpret_cast<const TSoftObjectPtr<>*>(ValuePtr));
	}

	// We have to specialize this because FSoftObjectProperty's TCppType is FSoftObjectPtr, which Blood doesn't recognize
	FBloodValue ReaderFuncSoftObject(const FProperty* ValueProp, const uint8* ValuePtr)
	{
		return ToBloodValue(*reinterpret_cast<const TSoftObjectPtr<>*>(ValuePtr));
	}

	FBloodValue ReaderFuncEnum(const FProperty* ValueProp, const uint8* ValuePtr)
	{
		if (const FEnumProperty* StructProp = CastField<FEnumProperty>(ValueProp))
		{
			return FBloodValue(StructProp->GetEnum(), ValuePtr);
		}
		return FBloodValue();
	}

	FBloodValue ReaderFuncStruct(const FProperty* ValueProp, const uint8* ValuePtr)
	{
		if (const FStructProperty* StructProp = CastField<FStructProperty>(ValueProp))
		{
			return FBloodValue(StructProp->Struct, ValuePtr);
		}
		return FBloodValue();
	}

	FPrecomputedMaps::FPrecomputedMaps()
	{
		ReaderMap = {
			{ FBoolProperty::StaticClass(), &ReaderFunc<FBoolProperty> },
			{ FByteProperty::StaticClass(), &ReaderFunc<FByteProperty> },
			{ FIntProperty::StaticClass(), &ReaderFunc<FIntProperty> },
			{ FInt64Property::StaticClass(), &ReaderFunc<FInt64Property> },
			{ FFloatProperty::StaticClass(), &ReaderFunc<FFloatProperty> },
			{ FDoubleProperty::StaticClass(), &ReaderFunc<FDoubleProperty> },
			{ FNameProperty::StaticClass(), &ReaderFunc<FNameProperty> },
			{ FStrProperty::StaticClass(), &ReaderFunc<FStrProperty> },
			{ FTextProperty::StaticClass(), &ReaderFunc<FTextProperty> },
			{ FClassProperty::StaticClass(), &ReaderFunc<FClassProperty> },
			{ FSoftClassProperty::StaticClass(), &ReaderFuncSoftClass },
			{ FObjectProperty::StaticClass(), &ReaderFunc<FObjectProperty> },
			{ FSoftObjectProperty::StaticClass(), &ReaderFuncSoftObject },
			{ FEnumProperty::StaticClass(), &ReaderFuncEnum },
			{ FStructProperty::StaticClass(), &ReaderFuncStruct }
		};
	}

	const FPrecomputedMaps& FPrecomputedMaps::Get()
	{
		static FPrecomputedMaps Maps;
		return Maps;
	}
}
