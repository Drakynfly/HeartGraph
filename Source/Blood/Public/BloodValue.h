// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "BloodData.h"

#include "BloodValue.generated.h"

USTRUCT(BlueprintType)
struct BLOOD_API FBloodValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FInstancedStruct Data;

	template <typename TBloodDataType>
	TBloodDataType GetValue() const
	{
		return Blood::TDataConverter<TBloodDataType>::Value(Data);
	}

	template <typename TBloodData>
	TArray<TBloodData> GetArrayValue() const
	{
		// @todo
		unimplemented();
		return {};
	}

	template <typename TBloodData>
	TSet<TBloodData> GetSetValue() const
	{
		// @todo
		unimplemented();
		return {};
	}

	template <typename TBloodDataKey, typename TBloodDataValue>
	TMap<TBloodDataKey, TBloodDataValue> GetMapValue() const
	{
		// @todo
		unimplemented();
		return {};
	}

	template <typename TBloodDataType>
	bool Is() const
	{
		if constexpr (Blood::TIsPoDWrapperStruct<TBloodDataType>::Value)
		{
			return GetUnderlyingType() == TBloodDataType::StaticStruct();
		}
		return GetUnderlyingType() == Blood::TDataConverter<TBloodDataType>::Type();
	}

	const UScriptStruct* GetUnderlyingType() const { return Data.GetScriptStruct(); }

	friend bool operator==(const FBloodValue& Lhs, const FBloodValue& Rhs)
	{
		return Lhs.Data == Rhs.Data;
	}

	friend bool operator!=(const FBloodValue& Lhs, const FBloodValue& Rhs)
	{
		return Lhs.Data != Rhs.Data;
	}
};

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

	template<typename TBloodData>
	static FBloodValue ToBloodValue(const TBloodData& Value)
	{
		FBloodValue OutValue;

		if constexpr (TIsPoDWrapperStruct<TBloodData>::Value)
		{
			OutValue.Data.InitializeAs(TBloodData::StaticStruct(), reinterpret_cast<const uint8*>(&Value));
		}
		else
		{
			OutValue.Data.InitializeAs(TDataConverter<TBloodData>::Type(), reinterpret_cast<const uint8*>(&Value));
		}

		return OutValue;
	}

	// Explicit specialization to prevent accidental recursion
	template<>
	FORCEINLINE FBloodValue ToBloodValue(const FBloodValue& Value)
	{
		return Value;
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

#if ALLOCATE_BLOOD_STATICS
	// Commonly needed values, stored here to avoid repeated conversion
	namespace Statics
	{
		// A null, typeless BloodValue, containing no data at all, but earmarked with FBloodWildcard so-as to not
		// confuse it with a malformed BloodValue
		static const FBloodValue Wildcard = ToBloodValue(FBloodWildcard());

		static const FBloodValue Boolean_False = ToBloodValue(false);
		static const FBloodValue Boolean_True = ToBloodValue(true);

		static const FBloodValue Float_Zero = ToBloodValue(0.f);
		static const FBloodValue Float_One = ToBloodValue(1.f);
		static const FBloodValue Double_Zero = ToBloodValue(0.0);
		static const FBloodValue Double_One = ToBloodValue(1.0);

		static const FBloodValue Name_None = ToBloodValue(Name_None);
		static const FBloodValue String_Empty = ToBloodValue(FString());
		static const FBloodValue Text_Empty = ToBloodValue(FText::GetEmpty());

		static const FBloodValue Vector3f_Zero = ToBloodValue(FVector3f::ZeroVector);
		static const FBloodValue Vector3f_One = ToBloodValue(FVector3f::OneVector);
		static const FBloodValue Vector3d_Zero = ToBloodValue(FVector::ZeroVector);
		static const FBloodValue Vector3d_One = ToBloodValue(FVector::OneVector);

		static const FBloodValue Object_Nullptr = ToBloodValue<TObjectPtr<UObject>>(nullptr);
		static const FBloodValue Class_Nullptr = ToBloodValue<TSubclassOf<UObject>>(nullptr);
		static const FBloodValue SoftObject_Nullptr = ToBloodValue<TSoftObjectPtr<UObject>>(nullptr);
		static const FBloodValue SoftClass_Nullptr = ToBloodValue<TSoftClassPtr<UObject>>(nullptr);
	}
#endif
}