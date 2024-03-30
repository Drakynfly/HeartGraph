// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "InstancedStruct.h"
#include "JsonObjectWrapper.h"
#include "UObject/Object.h"
#include "HeartJsonSerializer.generated.h"

USTRUCT()
struct FHeartSerializerIdentity_Json
{
	GENERATED_BODY()
};

UCLASS()
class HEARTJSON_API UHeartJsonLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Serialize an struct (and all its subobjects) into a flake. */
	UFUNCTION(BlueprintPure, Category = "Heart|FlakeLibrary", meta = (DisplayName = "Create Flake JSON (Struct)"))
	static FJsonObjectWrapper CreateFlake_Struct_Json(const FInstancedStruct& Struct);

	/** Serialize an object (and all its subobjects) into a flake. */
	UFUNCTION(BlueprintPure, Category = "Heart|FlakeLibrary", meta = (DisplayName = "Create Flake JSON (Object)"))
	static FJsonObjectWrapper CreateFlake_Json(const UObject* Object);

	/** Serialize an actor (and all its subobjects) into a flake. */
	UFUNCTION(BlueprintPure, Category = "Heart|FlakeLibrary", meta = (DisplayName = "Create Flake JSON (Actor)"))
	static FJsonObjectWrapper CreateFlake_Actor_Json(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Heart|FlakeLibrary", meta = (DeterminesOutputType = "ExpectedClass"))
	static UObject* ConstructObjectFromFlake_Json(const FJsonObjectWrapper& Flake, UObject* Outer, const UClass* ExpectedClass);

	/** Attempt to read a flake back into an actor. */
	UFUNCTION(BlueprintCallable, Category = "Heart|FlakeLibrary", meta = (WorldContext = "WorldContextObj", DeterminesOutputType = "ExpectedClass"))
	static AActor* ConstructActorFromFlake_Json(const FJsonObjectWrapper& Flake, UObject* WorldContextObj, const TSubclassOf<AActor> ExpectedClass);

	UFUNCTION(BlueprintCallable, Category = "Heart|FlakeLibrary")
	static FString ToString(const FJsonObjectWrapper& Json, bool bPretty);
};