// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "BloodPrecomputedMaps.h"

#include "BloodValue.h"

namespace Blood
{
	template <typename T>
	struct TReaderLambdaGen
	{
		static FFPropertyReadFunc Make()
		{
			return [](const FProperty* ValueProp, const uint8* ValuePtr)
				{
					return ToBloodValue(*reinterpret_cast<const typename T::TCppType*>(ValuePtr));
				};
		}
	};

	// We have to specialize this because FSoftClassProperty's TCppType is FSoftObjectPtr, which Blood doesn't recognize
	template<>
	struct TReaderLambdaGen<FSoftClassProperty>
	{
		static FFPropertyReadFunc Make()
		{
			return [](const FProperty* ValueProp, const uint8* ValuePtr)
				{
					return ToBloodValue(*reinterpret_cast<const TSoftObjectPtr<>*>(ValuePtr));
				};
		}
	};

	// We have to specialize this because FSoftObjectProperty's TCppType is FSoftObjectPtr, which Blood doesn't recognize
	template<>
	struct TReaderLambdaGen<FSoftObjectProperty>
	{
		static FFPropertyReadFunc Make()
		{
			return [](const FProperty* ValueProp, const uint8* ValuePtr)
				{
					return ToBloodValue(*reinterpret_cast<const TSoftObjectPtr<>*>(ValuePtr));
				};
		}
	};

	template<>
	struct TReaderLambdaGen<FEnumProperty>
	{
		static FFPropertyReadFunc Make()
		{
			return [](const FProperty* ValueProp, const uint8* ValuePtr)
				{
					if (const FEnumProperty* StructProp = CastField<FEnumProperty>(ValueProp))
					{
						return FBloodValue(StructProp->GetEnum(), ValuePtr);
					}
					return FBloodValue();
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
					if (const FStructProperty* StructProp = CastField<FStructProperty>(ValueProp))
					{
						return FBloodValue(StructProp->Struct, ValuePtr);
					}
					return FBloodValue();
				};
		}
	};

	FPrecomputedMaps::FPrecomputedMaps()
	{
		ReaderMap = {
			{ FBoolProperty::StaticClass(), TReaderLambdaGen<FBoolProperty>::Make() },
			{ FByteProperty::StaticClass(), TReaderLambdaGen<FByteProperty>::Make() },
			{ FIntProperty::StaticClass(), TReaderLambdaGen<FIntProperty>::Make() },
			{ FInt64Property::StaticClass(), TReaderLambdaGen<FInt64Property>::Make() },
			{ FFloatProperty::StaticClass(), TReaderLambdaGen<FFloatProperty>::Make() },
			{ FDoubleProperty::StaticClass(), TReaderLambdaGen<FDoubleProperty>::Make() },
			{ FNameProperty::StaticClass(), TReaderLambdaGen<FNameProperty>::Make() },
			{ FStrProperty::StaticClass(), TReaderLambdaGen<FStrProperty>::Make() },
			{ FTextProperty::StaticClass(), TReaderLambdaGen<FTextProperty>::Make() },
			{ FClassProperty::StaticClass(), TReaderLambdaGen<FClassProperty>::Make() },
			{ FSoftClassProperty::StaticClass(), TReaderLambdaGen<FSoftClassProperty>::Make() },
			{ FObjectProperty::StaticClass(), TReaderLambdaGen<FObjectProperty>::Make() },
			{ FSoftObjectProperty::StaticClass(), TReaderLambdaGen<FSoftObjectProperty>::Make() },
			{ FEnumProperty::StaticClass(), TReaderLambdaGen<FEnumProperty>::Make() },
			{ FStructProperty::StaticClass(), TReaderLambdaGen<FStructProperty>::Make() },
		};
	}

	const FPrecomputedMaps& FPrecomputedMaps::Get()
	{
		static FPrecomputedMaps Maps;
		return Maps;
	}
}