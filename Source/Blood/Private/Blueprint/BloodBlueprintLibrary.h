// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BloodValue.h"
#include "BloodContainer.h"
#include "BloodBlueprintLibrary.generated.h"

/**
 * Library exclusively for exposing Blood type functionality to blueprint. This class is not to be used by native code.
 */
UCLASS()
class UBloodBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// WARNING: This currently has no type-safety build in. It *will* crash if you misuse/abuse it.
	UFUNCTION(BlueprintPure, Category = "Blood|BlueprintUtils", CustomThunk, meta = (CustomStructureParam = "Value"))
	static void ReadProperty(const UObject* Object, FName Property, uint8& Value);

	UFUNCTION(BlueprintPure, Category = "Blood|BlueprintUtils", CustomThunk, meta = (BlueprintAutocast, CompactNodeTitle = "->", CustomStructureParam = "Property"))
	static FBloodValue PropertyToBlood(const uint8& Property);

	UFUNCTION(BlueprintPure, Category = "Blood|BlueprintUtils", CustomThunk, meta = (BlueprintAutocast, CompactNodeTitle = "->", CustomStructureParam = "Property"))
	static void BloodToProperty(const FBloodValue& Value, uint8& Property);

	UFUNCTION(BlueprintCallable, Category = "Blood|BlueprintUtils", CustomThunk, meta = (CompactNodeTitle = "<-", CustomStructureParam = "Property"))
	static bool AssignPropertyToBlood(UPARAM(ref) FBloodValue& Value, const uint8& Property);

	UFUNCTION(BlueprintCallable, Category = "Blood|BlueprintUtils", CustomThunk, meta = (CompactNodeTitle = "<-", CustomStructureParam = "Property"))
	static bool AssignBloodToProperty(UPARAM(ref) uint8& Property, const FBloodValue& Value);

	// Add or replace the value for a Name with a new Value
	UFUNCTION(BlueprintCallable, Category = "Blood|BlueprintUtils", meta = (CompactNodeTitle = "ADD"))
	static void AddBloodValue(UPARAM(ref) FBloodContainer& Container, FName Name, const FBloodValue& Value);

	// Remove a value for a Name
	UFUNCTION(BlueprintCallable, Category = "Blood|BlueprintUtils", meta = (CompactNodeTitle = "REMOVE"))
	static void Remove(UPARAM(ref) FBloodContainer& Container, FName Name);

	// Remove all values from the container
	UFUNCTION(BlueprintCallable, Category = "Blood|BlueprintUtils", meta = (CompactNodeTitle = "CLEAR"))
	static void Clear(UPARAM(ref) FBloodContainer& Container);

	// Get the value by a Name
	UFUNCTION(BlueprintPure, Category = "Blood|BlueprintUtils", meta = (CompactNodeTitle = "GET"))
	static void GetBloodValue(const FBloodContainer& Container, FName Name, FBloodValue& Value);

	// Find the value by a Name, if it exists
	UFUNCTION(BlueprintCallable, Category = "Blood|BlueprintUtils", meta = (CompactNodeTitle = "FIND"))
	static bool FindBloodValue(const FBloodContainer& Container, FName Name, FBloodValue& Value);

	// Does this container have a value for a Name
	UFUNCTION(BlueprintPure, Category = "Blood|BlueprintUtils", meta = (CompactNodeTitle = "CONTAINS"))
	static bool Contains(const FBloodContainer& Container, FName Name);

	UFUNCTION(BlueprintPure, Category = "Blood|BlueprintUtils", meta = (CompactNodeTitle = "NUM"))
	static int32 Num(const FBloodContainer& Container);

	UFUNCTION(BlueprintPure, Category = "Blood|BlueprintUtils", meta = (CompactNodeTitle = "IS EMPTY"))
	static bool IsEmpty(const FBloodContainer& Container);

private:
	DECLARE_FUNCTION(execReadProperty);
	DECLARE_FUNCTION(execPropertyToBlood);
	DECLARE_FUNCTION(execBloodToProperty);
	DECLARE_FUNCTION(execAssignPropertyToBlood);
	DECLARE_FUNCTION(execAssignBloodToProperty);
};