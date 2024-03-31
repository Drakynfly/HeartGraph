// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "BloodValue.h"

namespace Blood
{
	namespace Impl
	{
		// Internal-use-only class to move function implementations into .cpp file.
		class BLOOD_API FPropertyHelpers
		{
		public:
			static TObjectPtr<UField> GetFPropertyFieldTypeImpl(const FProperty* Prop);
			static bool WriteToFPropertyValuePtr(const FProperty* ValueProp, uint8* ValuePtr, const FBloodValue& Value);
			static FBloodValue ReadFromFPropertyValuePtr(const FProperty* ValueProp, const uint8* ValuePtr);
		};
	}

	static TObjectPtr<UField> GetFPropertyFieldType(const FProperty* Prop)
	{
		return Impl::FPropertyHelpers::GetFPropertyFieldTypeImpl(Prop);
	}

	// Write the data from a BloodValue to an FProperty
	static bool WriteToFProperty(const FProperty* Prop, uint8* ValuePtr, const FBloodValue& Value)
	{
		return Impl::FPropertyHelpers::WriteToFPropertyValuePtr(Prop, ValuePtr, Value);
	}

	// Write the data from a BloodValue to an FProperty
	static bool WriteToFProperty(UObject* Container, const FProperty* Prop, const FBloodValue& Value)
	{
		uint8* ValuePtr = Prop->ContainerPtrToValuePtr<uint8>(Container);
		return Impl::FPropertyHelpers::WriteToFPropertyValuePtr(Prop, ValuePtr, Value);
	}

	// Read the data from an FProperty to a BloodValue
	static FBloodValue ReadFromFProperty(const FProperty* ValueProp, const uint8* ValuePtr)
	{
		return Impl::FPropertyHelpers::ReadFromFPropertyValuePtr(ValueProp, ValuePtr);
	}

	// Read the data from an FProperty to a BloodValue
	static FBloodValue ReadFromFProperty(UObject* Container, const FProperty* Prop)
	{
		const uint8* ValuePtr = Prop->ContainerPtrToValuePtr<uint8>(Container);
		return Impl::FPropertyHelpers::ReadFromFPropertyValuePtr(Prop, ValuePtr);
	}

	// Read the value of the UPROPERTY by its name from an object. Works for both Blueprint Properties and Native UPROPS
	template <typename T>
	static T ReadUProperty(UObject* Container, const FName& PropName)
	{
		if (const FProperty* Property = Container->GetClass()->FindPropertyByName(PropName))
		{
			const FBloodValue BloodValue = ReadFromFProperty(Container, Property);
			return BloodValue.GetValue<T>();
		}

		return T();
	}
}