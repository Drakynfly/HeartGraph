// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "BloodData.h"
#include "PropertyBag.h"

#include "BloodValue.generated.h"

USTRUCT(BlueprintType)
struct BLOOD_API FBloodValue
{
	GENERATED_BODY()

	friend struct FBloodContainer; // @todo Temp, please improve API here, and remove this

	// Blank ctor
	FBloodValue() {}

	// Ctor from existing data. WARNING, this does not enforce, nor check, the validity of this data
	FBloodValue(FInstancedPropertyBag&& FormattedData)
	  : PropertyBag(MoveTemp(FormattedData)) {}

	// Ctor from single value
	template<typename TBloodData>
	explicit FBloodValue(const TBloodData& Value);

	// Ctor from a type and memory
	// @todo only works for structs right now (using BloodWrappers doesn't work)
	explicit FBloodValue(const UScriptStruct* Type, const uint8* Memory);

	// Ctor from single value
	explicit FBloodValue(const UEnum* Type, const uint8* Memory);

	template <typename TBloodData>
	auto GetValue() const;

	void Reset()
	{
		PropertyBag.Reset();
	}

	bool IsValid() const { return PropertyBag.IsValid(); }

	const uint8* GetMemory() const { return PropertyBag.GetValue().GetMemory(); }

	template <typename TBloodData>
	bool Is() const;

	// @todo this should really also work with wrapped types
	bool Is(const UField* Type) const;

	// Is this a single value
	bool IsSingle() const;

	// Is this a one-dimensional container, e.g, Arrays and Sets
	bool IsContainer1() const;

	// Is this a two-dimensional container, e.g, Maps
	bool IsContainer2() const;

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

template <typename TBloodData> FBloodValue::FBloodValue(const TBloodData& Value)
{
	if constexpr (TIsTMap<TBloodData>::Value)
	{
		static const TPair<FName, FName> Names{ Blood::Private::V0, Blood::Private::V1 };
		Blood::Write::Container2<TBloodData>(PropertyBag, Names, Value);
	}
	else if constexpr (TIsTArray<TBloodData>::Value)
	{
		Blood::Write::Container1<TBloodData>(PropertyBag, Blood::Private::V0, Value);
	}
	else if constexpr (TIsTSet<TBloodData>::Value)
	{
		Blood::Write::Container1<TBloodData>(PropertyBag, Blood::Private::V0, Value);
	}
	else
	{
		Blood::Write::Value(PropertyBag, Blood::Private::V0, Value);
	}
}

template <typename TBloodData> auto FBloodValue::GetValue() const
{
	if constexpr (TIsTMap<TBloodData>::Value)
	{
		static const TPair<FName, FName> Names{ Blood::Private::V0, Blood::Private::V1 };
		TBloodData Out;
		Blood::Read::Container2<TBloodData>(
			PropertyBag, Names, Out);
		return Out;
	}
	else if constexpr (TIsTArray<TBloodData>::Value)
	{
		TBloodData Out;
		Blood::Read::Container1<TBloodData>(PropertyBag, Blood::Private::V0, Out);
		return Out;
	}
	else if constexpr (TIsTSet<TBloodData>::Value)
	{
		TBloodData Out;
		Blood::Read::Container1<TBloodData>(PropertyBag, Blood::Private::V0, Out);
		return Out;
	}
	else
	{
		return Blood::Read::Value<TBloodData>(PropertyBag, Blood::Private::V0);
	}
}

template <typename TBloodData> bool FBloodValue::Is() const
{
	if constexpr (TIsTMap<TBloodData>::Value)
	{
		if (PropertyBag.GetNumPropertiesInBag() != 2) return false;
		const FPropertyBagPropertyDesc& Desc0 = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
		const FPropertyBagPropertyDesc& Desc1 = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[1];
		return Desc0.ContainerTypes.GetFirstContainerType() == EPropertyBagContainerType::Array &&
			   Desc1.ContainerTypes.GetFirstContainerType() == EPropertyBagContainerType::Array &&
			   Desc0.ValueType == Blood::TDataConverter<typename TBloodData::KeyType>::PropertyBagType() &&
			   Desc1.ValueType == Blood::TDataConverter<typename TBloodData::ValueType>::PropertyBagType() &&
			   Desc0.ValueTypeObject == Blood::TDataConverter<typename TBloodData::KeyType>::PropertyBagTypeObject() &&
			   Desc1.ValueTypeObject == Blood::TDataConverter<typename TBloodData::ValueType>::PropertyBagTypeObject();
	}
	else if constexpr (TIsTArray<TBloodData>::Value || TIsTSet<TBloodData>::Value)
	{
		if (PropertyBag.GetNumPropertiesInBag() != 1) return false;
		const FPropertyBagPropertyDesc& Desc = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
		return Desc.ContainerTypes.GetFirstContainerType() == EPropertyBagContainerType::Array &&
			   Desc.ValueType == Blood::TDataConverter<typename TBloodData::ElementType>::PropertyBagType() &&
			   Desc.ValueTypeObject == Blood::TDataConverter<typename TBloodData::ElementType>::PropertyBagTypeObject();
	}
	else
	{
		if constexpr (Blood::TIsPoDWrapperStruct<TBloodData>::Value)
		{
			if (PropertyBag.GetNumPropertiesInBag() != 1) return false;
			const FPropertyBagPropertyDesc& Desc = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
			return Desc.ContainerTypes.GetFirstContainerType() == EPropertyBagContainerType::None &&
			   Desc.ValueType == Blood::TDataConverter<decltype(TBloodData::Value)>::PropertyBagType() &&
			   Desc.ValueTypeObject == Blood::TDataConverter<decltype(TBloodData::Value)>::PropertyBagTypeObject();
		}
		else
		{
			if (PropertyBag.GetNumPropertiesInBag() != 1) return false;
			const FPropertyBagPropertyDesc& Desc = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
			return Desc.ContainerTypes.IsEmpty() &&
			   Desc.ValueType == Blood::TDataConverter<TBloodData>::PropertyBagType() &&
			   Desc.ValueTypeObject == Blood::TDataConverter<TBloodData>::PropertyBagTypeObject();;
		}
	}
}

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