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
			{ FStructProperty::StaticClass(), TReaderLambdaGen<FStructProperty>::Make() },
		};
	}

	const FPrecomputedMaps& FPrecomputedMaps::Get()
	{
		static FPrecomputedMaps Maps;
		return Maps;
	}
}
