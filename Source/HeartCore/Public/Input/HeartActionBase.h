// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Input/HeartInputActivation.h"

#include "HeartActionBase.generated.h"

struct FHeartManualEvent;

namespace Heart::Action
{
	// System-level flags for optional behavior in actions
	enum EExecutionFlags
	{
		NoFlags = 0,

		// This action is being ran as a Redo of an previously undone action
		IsRedo = 1 << 0,

		// Prevent this action from being recorded by action history
		DisallowRecord = 1 << 1,

		// Record this action, even if it doesn't have a reason to
		ForceRecord = 1 << 2,
	};

	struct FArguments
	{
		UObject* Target = nullptr;
		FHeartInputActivation Activation;
		UObject* Payload = nullptr;
		EExecutionFlags Flags = NoFlags;
	};
}
ENUM_CLASS_FLAGS(Heart::Action::EExecutionFlags)


/**
 *
 */
UCLASS(Abstract)
class HEARTCORE_API UHeartActionBase : public UObject
{
	GENERATED_BODY()

public:
	/** Creates a immediately executes an action in a "fire and forget" manner */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase")
	static bool QuickExecuteGraphAction(TSubclassOf<UHeartActionBase> Class, UObject* Target, const FHeartManualEvent& Activation);

	/** Creates a immediately executes an action in a "fire and forget" manner */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase")
	static bool QuickExecuteGraphActionWithPayload(TSubclassOf<UHeartActionBase> Class, UObject* Target, const FHeartManualEvent& Activation, UObject* Payload);

	template <typename THeartGraphAction>
	static THeartGraphAction* CreateGraphAction(const TSubclassOf<UHeartActionBase> Class)
	{
		static_assert(TIsDerivedFrom<THeartGraphAction, UHeartActionBase>::Value, TEXT("THeartGraphAction must be a UHeartActionBase"));
		return Cast<THeartGraphAction>(CreateGraphAction(Class));
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase", meta = (DeterminesOutputType = Class))
	static UHeartActionBase* CreateGraphAction(TSubclassOf<UHeartActionBase> Class);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase", meta = (DeterminesOutputType = Class))
	static bool ExecuteGraphAction(UHeartActionBase* Action, UObject* Target, const FHeartManualEvent& Activation);

public:
	virtual FText GetDescription(const UObject* Target) const PURE_VIRTUAL(UHeartActionBase::GetDescription, return FText::GetEmpty(); )
	virtual bool CanExecute(const UObject* Target) const PURE_VIRTUAL(UHeartActionBase::CanExecute, return false; )
	virtual bool Execute(const Heart::Action::FArguments& Arguments) PURE_VIRTUAL(UHeartActionBase::Execute, return false; )
	virtual bool CanUndo(UObject* Target) const { return false; }
	virtual bool Undo(UObject* Target) { return false; }
};