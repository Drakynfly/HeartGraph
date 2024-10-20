// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "BloodValue.h"
#include "BloodProperty.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BloodValue)

FBloodValue::FBloodValue(const UScriptStruct* Type, const uint8* Memory)
{
	// Init property
	PropertyBag.AddProperty(
		Blood::Private::V0,
		EPropertyBagPropertyType::Struct,
		Type);

	const FPropertyBagPropertyDesc& Desc = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];

	Desc.CachedProperty->CopyCompleteValue_InContainer(PropertyBag.GetMutableValue().GetMemory(), Memory);
}

FBloodValue::FBloodValue(const UScriptStruct* Type, const TConstArrayView<const uint8*> Memory)
{
	// Init property
	PropertyBag.AddContainerProperty(
		Blood::Private::V0,
		EPropertyBagContainerType::Array,
		EPropertyBagPropertyType::Struct,
		Type);

	const FPropertyBagPropertyDesc& Desc = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
	auto&& BetterBeArray = PropertyBag.GetMutableArrayRef(Blood::Private::V0);
	check(BetterBeArray.HasValue());

	FPropertyBagArrayRef& ArrayRef = BetterBeArray.GetValue();

	ArrayRef.AddValues(Memory.Num());
	for (int32 i = 0; i < Memory.Num(); ++i)
	{
		Desc.CachedProperty->CopyCompleteValue(ArrayRef.GetRawPtr(i), Memory[i]);
	}
}

FBloodValue::FBloodValue(const UEnum* Type, const uint8* Memory)
{
	// Init property
	PropertyBag.AddProperty(
		Blood::Private::V0,
		EPropertyBagPropertyType::Enum,
		Type);

	const FPropertyBagPropertyDesc& Desc = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];

	Desc.CachedProperty->CopyCompleteValue_InContainer(PropertyBag.GetMutableValue().GetMemory(), Memory);
}

FInstancedStruct FBloodValue::GetStruct() const
{
	auto Res = PropertyBag.GetValueStruct(Blood::Private::V0);
	if (Res.HasValue())
	{
		return FInstancedStruct(Res.GetValue());
	}
	return FInstancedStruct();
}

bool FBloodValue::Is(const UField* Type) const
{
	const FPropertyBagPropertyDesc& Desc = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
	return Desc.ValueTypeObject == Type;
}

bool FBloodValue::IsSingle() const
{
	if (PropertyBag.GetNumPropertiesInBag() != 1) return false;
	const FPropertyBagPropertyDesc& Desc = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
	return Desc.ContainerTypes.IsEmpty();
}

bool FBloodValue::IsContainer1() const
{
	if (PropertyBag.GetNumPropertiesInBag() != 1) return false;
	const FPropertyBagPropertyDesc& Desc = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
	return Desc.ContainerTypes.GetFirstContainerType() == EPropertyBagContainerType::Array;
}

bool FBloodValue::IsContainer2() const
{
	if (PropertyBag.GetNumPropertiesInBag() != 2) return false;
	const FPropertyBagPropertyDesc& Desc0 = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
	const FPropertyBagPropertyDesc& Desc1 = PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[1];
	return Desc0.ContainerTypes.GetFirstContainerType() == EPropertyBagContainerType::Array &&
		   Desc1.ContainerTypes.GetFirstContainerType() == EPropertyBagContainerType::Array;
}

#if ALLOCATE_BLOOD_STATICS
namespace Blood
{
	// A null, typeless BloodValue, containing no data at all, but earmarked with FBloodWildcard so-as to not
	// confuse it with a malformed BloodValue
	const FBloodValue FStatics::Wildcard = ToBloodValue(FBloodWildcard());

	const FBloodValue FStatics::Boolean_False = ToBloodValue(false);
	const FBloodValue FStatics::Boolean_True = ToBloodValue(true);

	const FBloodValue FStatics::Float_Zero = ToBloodValue(0.f);
	const FBloodValue FStatics::Float_One = ToBloodValue(1.f);
	const FBloodValue FStatics::Double_Zero = ToBloodValue(0.0);
	const FBloodValue FStatics::Double_One = ToBloodValue(1.0);

	const FBloodValue FStatics::Name_None = ToBloodValue(Name_None);
	const FBloodValue FStatics::String_Empty = ToBloodValue(FString());
	const FBloodValue FStatics::Text_Empty = ToBloodValue(FText::GetEmpty());

	const FBloodValue FStatics::Vector3f_Zero = ToBloodValue(FVector3f::ZeroVector);
	const FBloodValue FStatics::Vector3f_One = ToBloodValue(FVector3f::OneVector);
	const FBloodValue FStatics::Vector3d_Zero = ToBloodValue(FVector::ZeroVector);
	const FBloodValue FStatics::Vector3d_One = ToBloodValue(FVector::OneVector);

	const FBloodValue FStatics::Object_Nullptr = ToBloodValue<TObjectPtr<UObject>>(nullptr);
	const FBloodValue FStatics::Class_Nullptr = ToBloodValue<TSubclassOf<UObject>>(nullptr);
	const FBloodValue FStatics::SoftObject_Nullptr = ToBloodValue<TSoftObjectPtr<UObject>>(nullptr);
	const FBloodValue FStatics::SoftClass_Nullptr = ToBloodValue<TSoftClassPtr<UObject>>(nullptr);
}
#endif