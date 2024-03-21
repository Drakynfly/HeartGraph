// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "UI/HeartInputActivation.h"

#include "HeartGraphActionBase.generated.h"

struct FHeartManualEvent;

namespace Heart::Action
{
	enum EExecutionFlags
	{
		NoFlags = 0,
	};

	struct FArguments
	{
		FHeartInputActivation Activation;
		UObject* Payload = nullptr;
		EExecutionFlags Flags;
	};
}


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

public:
	virtual FText GetDescription(const UObject* Object) const PURE_VIRTUAL(UHeartGraphActionBase::GetDescription, return FText::GetEmpty(); )
	virtual bool CanExecute(const UObject* Object) const PURE_VIRTUAL(UHeartGraphActionBase::CanExecute, return false; )
	virtual bool Execute(UObject* Object, const Heart::Action::FArguments& Arguments) PURE_VIRTUAL(UHeartGraphActionBase::Execute, return false; )
};