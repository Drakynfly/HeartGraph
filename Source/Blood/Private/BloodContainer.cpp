// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "BloodContainer.h"
#include "BloodValue.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BloodContainer)

// #@todo this is all... very questionable

void FBloodContainer::AddBloodValue(const FName Name, const FBloodValue& Value)
{
	if (Value.IsContainer2())
	{
		if (Value.PropertyBag.GetNumPropertiesInBag() != 2)
		{
			// Invalid Descs number, abort
			return;
		}

		const FPropertyBagPropertyDesc& Desc_Src_0 = Value.PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
		const FPropertyBagPropertyDesc& Desc_Src_1 = Value.PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[1];

		FPropertyBagPropertyDesc DescV0 = Desc_Src_0;
		FPropertyBagPropertyDesc DescV1 = Desc_Src_1;

		CreateMapNames(Name, DescV0.Name, DescV1.Name);

		PropertyBag.AddProperties({DescV0, DescV1});

		const uint8* Memory = Value.GetMemory();
		PropertyBag.SetValue(DescV0.Name, Desc_Src_0.CachedProperty, Memory);
		PropertyBag.SetValue(DescV1.Name, Desc_Src_1.CachedProperty, Memory);
	}
	else
	{
		if (Value.PropertyBag.GetNumPropertiesInBag() != 1)
		{
			// Invalid Descs number, abort
			return;
		}

		const FPropertyBagPropertyDesc& Desc_Src = Value.PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
		FPropertyBagPropertyDesc Desc_Copy = Desc_Src;
		Desc_Copy.Name = Name;

		PropertyBag.AddProperties({Desc_Copy});
		PropertyBag.SetValue(Name, Desc_Src.CachedProperty, Value.GetMemory());
	}
}

void FBloodContainer::Remove(const FName Name)
{
	TArray<FName, TInlineAllocator<3>> Names;
	Names.SetNum(3);
	Names[0] = Name;
	CreateMapNames(Name, Names[1], Names[2]);
	PropertyBag.RemovePropertiesByName(Names);
}

void FBloodContainer::Clear()
{
	PropertyBag.Reset();
}

TOptional<FBloodValue> FBloodContainer::GetBloodValue(const FName Name) const
{
	if (auto&& ExactDesc = PropertyBag.FindPropertyDescByName(Name))
	{
		FInstancedPropertyBag Temp;
		FPropertyBagPropertyDesc CopyDesc = *ExactDesc;
		CopyDesc.Name = Blood::Private::V0;
		Temp.AddProperties({CopyDesc});
		Temp.SetValue(Blood::Private::V0, ExactDesc->CachedProperty, PropertyBag.GetValue().GetMemory());
		return FBloodValue(MoveTemp(Temp));
	}

	// If access as single value/container1 fails, try access as container2
	{
		FName KeyName, ValueName;
		CreateMapNames(Name, KeyName, ValueName);
		auto&& DescV0 = PropertyBag.FindPropertyDescByName(KeyName);
		auto&& DescV1 = PropertyBag.FindPropertyDescByName(ValueName);
		if (DescV0 && DescV1)
		{
			FInstancedPropertyBag Temp;
			TArray<FPropertyBagPropertyDesc> CopyDescs{*DescV0, *DescV1};
			CopyDescs[0].Name = Blood::Private::V0;
			CopyDescs[1].Name = Blood::Private::V1;
			Temp.AddProperties(CopyDescs);
			Temp.SetValue(Blood::Private::V0, DescV0->CachedProperty, PropertyBag.GetValue().GetMemory());
			Temp.SetValue(Blood::Private::V1, DescV1->CachedProperty, PropertyBag.GetValue().GetMemory());
			return FBloodValue(MoveTemp(Temp));
		}
	}

	return {};
}

bool FBloodContainer::Contains(const FName Name) const
{
	return PropertyBag.FindPropertyDescByName(Name) != nullptr;
}

int32 FBloodContainer::Num() const
{
	return PropertyBag.GetNumPropertiesInBag();
}

bool FBloodContainer::IsEmpty() const
{
	return PropertyBag.GetNumPropertiesInBag() == 0;
}

void FBloodContainer::CreateMapNames(const FName Base, FName& KeyName, FName& ValueName)
{
	FNameBuilder NameBuilder[2];
	Base.ToString(NameBuilder[0]);
	Base.ToString(NameBuilder[1]);
	NameBuilder[0].Append(TEXT("__V0"));
	NameBuilder[1].Append(TEXT("__V1"));
	KeyName = FName(NameBuilder[0]);
	ValueName = FName(NameBuilder[1]);
}