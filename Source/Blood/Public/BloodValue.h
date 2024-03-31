// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "BloodData.h"
#include "PropertyBag.h"

#include "BloodValue.generated.h"

USTRUCT(BlueprintType)
struct BLOOD_API FBloodValue
{
	GENERATED_BODY()

	// Blank ctor
	FBloodValue() {}

	// Ctor from single value
	template<typename TBloodData>
	explicit FBloodValue(const TBloodData& Value)
	{
		Blood::Write::Value(PropertyBag, Value);
	}

	// Ctor from single value
	// @todo only works for structs right now (using BloodWrappers doesnt work)
	explicit FBloodValue(const UScriptStruct* Type, const uint8* Memory);

	// Ctor from single value
	explicit FBloodValue(const UEnum* Type, const uint8* Memory);

	// Ctor from TArray
	template<typename TBloodData>
	explicit FBloodValue(const TArray<TBloodData>& Value)
	{
		FBloodValue OutValue;
		Blood::Write::Container1<TArray, TBloodData>(PropertyBag, Value);
	}

	// Ctor from TSet
	template<typename TBloodData>
	explicit FBloodValue(const TSet<TBloodData>& Value)
	{
		Blood::Write::Container1<TSet, TBloodData>(PropertyBag, Value);
	}

	// Ctor from TMap
	template<typename TBloodDataValue, typename TBloodDataKey>
	explicit FBloodValue(const TMap<TBloodDataValue, TBloodDataKey>& Value)
	{
		Blood::Write::Container2<TMap, TBloodDataValue, TBloodDataKey>(PropertyBag, Value);
	}

	template <typename TBloodDataType>
	auto GetValue() const
	{
		if constexpr (TIsTMap<TBloodDataType>::Value)
		{
			TMap<typename TBloodDataType::KeyType, typename TBloodDataType::ValueType> Out;
			Blood::Read::Container2<TMap, typename TBloodDataType::KeyType, typename TBloodDataType::ValueType>(PropertyBag, Out);
			return Out;
		}
		else if constexpr (TIsTArray<TBloodDataType>::Value)
		{
			TArray<typename TBloodDataType::ElementType> Out;
			Blood::Read::Container1<TArray, typename TBloodDataType::ElementType>(PropertyBag, Out);
			return Out;
		}
		else if constexpr (TIsTSet<TBloodDataType>::Value)
		{
			TSet<typename TBloodDataType::ElementType> Out;
			Blood::Read::Container1<TSet, typename TBloodDataType::ElementType>(PropertyBag, Out);
			return Out;
		}
		else
		{
			return Blood::Read::Value<TBloodDataType>(PropertyBag);
		}
	}

	void Reset()
	{
		PropertyBag.Reset();
	}

	bool IsValid() const { return PropertyBag.IsValid(); }

	const uint8* GetMemory() const { return PropertyBag.GetValue().GetMemory(); }

	template <typename TBloodDataType>
	bool Is() const
	{
		const FPropertyBagPropertyDesc* Desc0 = PropertyBag.FindPropertyDescByName(Blood::Private::V0);

		if constexpr (TIsTMap<TBloodDataType>::Value)
		{
			const FPropertyBagPropertyDesc* Desc1 = PropertyBag.FindPropertyDescByName(Blood::Private::V1);
			return Desc0->ContainerTypes.GetFirstContainerType() == EPropertyBagContainerType::Array &&
				   Desc1->ContainerTypes.GetFirstContainerType() == EPropertyBagContainerType::Array &&
				   Desc0->ValueType == Blood::TDataConverter<typename TBloodDataType::KeyType>::PropertyBagType() &&
				   Desc1->ValueType == Blood::TDataConverter<typename TBloodDataType::ValueType>::PropertyBagType() &&
				   Desc0->ValueTypeObject == Blood::TDataConverter<typename TBloodDataType::KeyType>::PropertyBagTypeObject() &&
				   Desc1->ValueTypeObject == Blood::TDataConverter<typename TBloodDataType::ValueType>::PropertyBagTypeObject();
		}
		else if constexpr (TIsTArray<TBloodDataType>::Value || TIsTSet<TBloodDataType>::Value)
		{
			return Desc0->ContainerTypes.GetFirstContainerType() == EPropertyBagContainerType::Array &&
				   Desc0->ValueType == Blood::TDataConverter<typename TBloodDataType::ElementType>::PropertyBagType() &&
				   Desc0->ValueTypeObject == Blood::TDataConverter<typename TBloodDataType::ElementType>::PropertyBagTypeObject();
		}
		else
		{
			if constexpr (Blood::TIsPoDWrapperStruct<TBloodDataType>::Value)
			{
				return Desc0->ContainerTypes.GetFirstContainerType() == EPropertyBagContainerType::None &&
				   Desc0->ValueType == Blood::TDataConverter<decltype(TBloodDataType::Value)>::PropertyBagType() &&
				   Desc0->ValueTypeObject == Blood::TDataConverter<decltype(TBloodDataType::Value)>::PropertyBagTypeObject();
			}
			else
			{
				return Desc0->ContainerTypes.GetFirstContainerType() == EPropertyBagContainerType::None &&
				   Desc0->ValueType == Blood::TDataConverter<TBloodDataType>::PropertyBagType() &&
				   Desc0->ValueTypeObject == Blood::TDataConverter<TBloodDataType>::PropertyBagTypeObject();
			}
		}
	}

	// @todo this should really also work with wrapped types
	bool Is(const UField* Type) const
	{
		const FPropertyBagPropertyDesc* Desc = PropertyBag.FindPropertyDescByName(Blood::Private::V0);
		return Desc->ValueTypeObject == Type;
	}

	friend bool operator==(const FBloodValue& Lhs, const FBloodValue& Rhs)
	{
		return Lhs.PropertyBag.GetValue().Get<const FInstancedStruct>().Identical(Rhs.PropertyBag.GetValue().GetPtr<const FInstancedStruct>(), 0);
	}

	friend bool operator!=(const FBloodValue& Lhs, const FBloodValue& Rhs)
	{
		return !(Lhs == Rhs);
	}

private:
	UPROPERTY(EditAnywhere)
	FInstancedPropertyBag PropertyBag;
};

namespace Blood
{
	template<typename TBloodData>
	static FBloodValue ToBloodValue(const TBloodData& Value)
	{
		return FBloodValue(Value);
	}

	// Explicit specialization to prevent accidental recursion
	template<>
	FORCEINLINE FBloodValue ToBloodValue(const FBloodValue& Value)
	{
		return Value;
	}

#if ALLOCATE_BLOOD_STATICS
	// Commonly needed values, stored here to avoid repeated conversion
	class FStatics
	{
		// A null, typeless BloodValue, containing no data at all, but earmarked with FBloodWildcard so-as to not
		// confuse it with a malformed BloodValue
		static const FBloodValue Wildcard;

		static const FBloodValue Boolean_False;
		static const FBloodValue Boolean_True;

		static const FBloodValue Float_Zero;
		static const FBloodValue Float_One;
		static const FBloodValue Double_Zero;
		static const FBloodValue Double_One;

		static const FBloodValue Name_None;
		static const FBloodValue String_Empty;
		static const FBloodValue Text_Empty;

		static const FBloodValue Vector3f_Zero;
		static const FBloodValue Vector3f_One;
		static const FBloodValue Vector3d_Zero;
		static const FBloodValue Vector3d_One;

		static const FBloodValue Object_Nullptr;
		static const FBloodValue Class_Nullptr;
		static const FBloodValue SoftObject_Nullptr;
		static const FBloodValue SoftClass_Nullptr;
	};
#endif
}