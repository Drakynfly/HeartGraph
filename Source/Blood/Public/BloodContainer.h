// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

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

	// Add or replace the value for a Name with a new Value
	void AddBloodValue(FName Name, const FBloodValue& Value);

	// Remove a value for a Name
	void Remove(FName Name);

	// Remove all values
	void Clear();

	// Get a value as a Blood Value if present
	TOptional<FBloodValue> GetBloodValue(FName Name) const;

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