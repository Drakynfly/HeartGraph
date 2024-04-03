// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "BloodData.h"
#include "PropertyBag.h"
#include "BloodContainer.generated.h"

struct FBloodValue;

/**
 * Blood Containers are a collection of Blood Values. They are essentially a dynamic struct, and they can hold any number
 * of values of any type, designated by FName labels.
 * Internally implemented using FInstancedPropertyBag.
 */
USTRUCT(BlueprintType)
struct BLOOD_API FBloodContainer
{
	GENERATED_BODY()

	FBloodContainer() {}

	// Copy data from an existing property bag
	FBloodContainer(const FInstancedPropertyBag& PropertyBag)
	  : PropertyBag(PropertyBag) {}

	// Move data from an existing property bag
	FBloodContainer(FInstancedPropertyBag&& PropertyBag)
	  : PropertyBag(MoveTemp(PropertyBag)) {}

	// Add or replace the value for a Name with a new Value
	void AddBloodValue(FName Name, const FBloodValue& Value);

	template<typename TBloodData>
	void Add(FName Name, const TBloodData& Value);

	// Remove a value for a Name
	void Remove(FName Name);

	// Remove all values
	void Clear();

	// Get a value as a Blood Value if present
	TOptional<FBloodValue> GetBloodValue(FName Name) const;

	template<typename TBloodData>
	TBloodData Get(const FName Name) const;

	// Does this container have a value for a Name
	bool Contains(FName Name) const;

	int32 Num() const;

	bool IsEmpty() const;

	friend FArchive& operator<<(FArchive& Ar, FBloodContainer& Container)
	{
		Container.PropertyBag.Serialize(Ar);
		return Ar;
	}

private:
	UPROPERTY()
	FInstancedPropertyBag PropertyBag;
};

template <typename TBloodData> void FBloodContainer::Add(FName Name, const TBloodData& Value)
{
	if constexpr (TIsTMap<TBloodData>::Value)
	{
		FNameBuilder NameBuilder[2];
		Name.ToString(NameBuilder[0]);
		Name.ToString(NameBuilder[1]);
		NameBuilder[0].Append(TEXT("__V0"));
		NameBuilder[1].Append(TEXT("__V1"));
		const TPair<FName, FName> Names{ NameBuilder[0], NameBuilder[1] };
		Blood::Write::Container2<TMap, typename TBloodData::KeyType, typename TBloodData::ValueType>(PropertyBag, Names, Value);
	}
	else if constexpr (TIsTArray<TBloodData>::Value)
	{
		Blood::Write::Container1<TArray, typename TBloodData::ElementType>(PropertyBag, Name, Value);
	}
	else if constexpr (TIsTSet<TBloodData>::Value)
	{
		Blood::Write::Container1<TSet, typename TBloodData::ElementType>(PropertyBag, Name, Value);
	}
	else
	{
		Blood::Write::Value(PropertyBag, Name, Value);
	}
}

template <typename TBloodData> TBloodData FBloodContainer::Get(const FName Name) const
{
	if constexpr (TIsTMap<TBloodData>::Value)
	{
		FNameBuilder NameBuilder[2];
		Name.ToString(NameBuilder[0]);
		Name.ToString(NameBuilder[1]);
		NameBuilder[0].Append(TEXT("__V0"));
		NameBuilder[1].Append(TEXT("__V1"));
		const TPair<FName, FName> Names{ NameBuilder[0], NameBuilder[1] };
		TBloodData Out;
		Blood::Read::Container2<TMap, typename TBloodData::KeyType, typename TBloodData::ValueType>(
			PropertyBag, Names, Out);
		return Out;
	}
	else if constexpr (TIsTArray<TBloodData>::Value)
	{
		TBloodData Out;
		Blood::Read::Container1<TArray, typename TBloodData::ElementType>(PropertyBag, Name, Out);
		return Out;
	}
	else if constexpr (TIsTSet<TBloodData>::Value)
	{
		TBloodData Out;
		Blood::Read::Container1<TSet, typename TBloodData::ElementType>(PropertyBag, Name, Out);
		return Out;
	}
	else
	{
		return Blood::Read::Value<TBloodData>(PropertyBag, Name);
	}
}