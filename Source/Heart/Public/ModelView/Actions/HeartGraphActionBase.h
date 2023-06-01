// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "UI/HeartInputActivation.h"

#include "HeartGraphActionBase.generated.h"

struct FHeartManualEvent;

/**
 *
 */
UCLASS(Abstract, Const)
class HEART_API UHeartGraphActionBase : public UObject
{
	GENERATED_BODY()

public:
	/** Creates a immediately executes an action in a "fire and forget" manner */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase")
	static bool QuickExecuteGraphAction(TSubclassOf<UHeartGraphActionBase> Class, UObject* Target, const FHeartManualEvent& Activation);

	/** Creates a immediately executes an action in a "fire and forget" manner */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase")
	static bool QuickExecuteGraphActionWithPayload(TSubclassOf<UHeartGraphActionBase> Class, UObject* Target, const FHeartManualEvent& Activation, UObject* Payload);

	template <typename THeartGraphAction>
	static THeartGraphAction* CreateGraphAction(const TSubclassOf<UHeartGraphActionBase> Class)
	{
		static_assert(TIsDerivedFrom<THeartGraphAction, UHeartGraphActionBase>::Value, TEXT("THeartGraphAction must be a UHeartGraphActionBase"));
		return Cast<THeartGraphAction>(CreateGraphAction(Class));
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase", meta = (DeterminesOutputType = Class))
	static UHeartGraphActionBase* CreateGraphAction(TSubclassOf<UHeartGraphActionBase> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase", meta = (DeterminesOutputType = Class))
	static bool ExecuteGraphAction(UHeartGraphActionBase* Action, UObject* Target, const FHeartManualEvent& Activation);

protected:
	bool Execute(UObject* Object, const FHeartInputActivation& Activation);

public:
	virtual FText GetDescription(const UObject* Object) const PURE_VIRTUAL(UHeartGraphActionBase::GetDescription, return FText(); )
	virtual bool CanExecute(const UObject* Object) const PURE_VIRTUAL(UHeartGraphActionBase::CanExecute, return false; )
	virtual bool Execute(UObject* Object, const FHeartInputActivation& Activation, UObject* ContextObject) PURE_VIRTUAL(UHeartGraphActionBase::Execute, return false; )
};
