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
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase", meta = (DeterminesOutputType = Class))
	static UHeartGraphActionBase* CreateGraphAction(TSubclassOf<UHeartGraphActionBase> Class);

	virtual bool Execute(UObject* Object, const FHeartInputActivation& Activation) PURE_VIRTUAL(UHeartGraphActionBase::Execute, return false; )
};
