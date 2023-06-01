﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BloodValue.h"
#include "BloodBlueprintLibrary.generated.h"

UCLASS()
class BLOOD_API UBloodBlueprintLibrary : public UBlueprintFunctionLibrary
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
	static bool AssignPropertyToBlood(UPARAM(ref)FBloodValue& Value, const uint8& Property);

	UFUNCTION(BlueprintCallable, Category = "Blood|BlueprintUtils", CustomThunk, meta = (CompactNodeTitle = "<-", CustomStructureParam = "Property"))
	static bool AssignBloodToProperty(UPARAM(ref)uint8& Property, const FBloodValue& Value);

private:
	DECLARE_FUNCTION(execReadProperty);
	DECLARE_FUNCTION(execPropertyToBlood);
	DECLARE_FUNCTION(execBloodToProperty);
	DECLARE_FUNCTION(execAssignPropertyToBlood);
	DECLARE_FUNCTION(execAssignBloodToProperty);
};
