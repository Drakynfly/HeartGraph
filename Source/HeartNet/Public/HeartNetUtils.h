// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartNetUtils.generated.h"

/**
 *
 */
UCLASS()
class HEARTNET_API UHeartNetUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// NOTE: these SubObjectList utils will likely be added to UE engine eventually.

	// Is an object replicated by this actor.
	UFUNCTION(BlueprintPure, Category = "Heart|Utils|Net", meta = (DefaultToSelf = Actor))
	static bool IsObjectInActorReplicateSubObjectList(const AActor* Actor, const UObject* Object);

	// Adds an object to the actor's SubObjectList, so it can be replicated.
	// The actor must be somewhere up the objects outer chain, and have ReplicateUsingRegisteredSubObjectList enabled
	UFUNCTION(BlueprintCallable, Category = "Heart|Utils|Net", meta = (DefaultToSelf = Actor))
	static bool AddObjectToActorReplicateSubObjectList(AActor* Actor, UObject* Object);

	// Removes an object from the actor's SubObjectList.
	UFUNCTION(BlueprintCallable, Category = "Heart|Utils|Net", meta = (DefaultToSelf = Actor))
	static bool RemoveObjectFromActorReplicateSubObjectList(AActor* Actor, UObject* Object);
};