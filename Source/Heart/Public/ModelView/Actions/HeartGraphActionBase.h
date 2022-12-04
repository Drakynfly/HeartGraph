// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "UI/HeartInputActivation.h"
#include "HeartGraphActionBase.generated.h"

/**
 *
 */
UCLASS(Abstract)
class HEART_API UHeartGraphActionBase : public UObject
{
	GENERATED_BODY()

public:
	/** Creates a immediately executes an action in a "fire and forget" manner */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase")
	static bool QuickExecuteGraphAction(TSubclassOf<UHeartGraphActionBase> Class, UObject* Target, const FHeartInputActivation& Activation);

	/** Creates a immediately executes an action in a "fire and forget" manner */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase")
	static bool QuickExecuteGraphActionWithPayload(TSubclassOf<UHeartGraphActionBase> Class, UObject* Target, const FHeartInputActivation& Activation, UObject* Payload);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase", meta = (DeterminesOutputType = Class))
	static UHeartGraphActionBase* CreateGraphAction(TSubclassOf<UHeartGraphActionBase> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase", meta = (DeterminesOutputType = Class))
	static bool ExecuteGraphAction(UHeartGraphActionBase* Action, UObject* Target, const FHeartInputActivation& Activation);

	bool Execute(UObject* Object, const FHeartInputActivation& Activation);
	virtual bool Execute(UObject* Object, const FHeartInputActivation& Activation, UObject* ContextObject) PURE_VIRTUAL(UHeartGraphActionBase::Execute, return false; )
};
