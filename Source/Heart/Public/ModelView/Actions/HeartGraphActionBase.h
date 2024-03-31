// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "UI/HeartInputActivation.h"

#include "HeartGraphActionBase.generated.h"

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

		friend FArchive& operator<<(FArchive& Ar, FArguments& Arguments)
		{
			return Ar
				<< Arguments.Target
				<< Arguments.Activation
				<< Arguments.Payload
				<< *reinterpret_cast<uint8*>(&Arguments.Flags);
		}
	};
}
ENUM_CLASS_FLAGS(Heart::Action::EExecutionFlags)


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
	virtual FText GetDescription(const UObject* Target) const PURE_VIRTUAL(UHeartGraphActionBase::GetDescription, return FText::GetEmpty(); )
	virtual bool CanExecute(const UObject* Target) const PURE_VIRTUAL(UHeartGraphActionBase::CanExecute, return false; )
	virtual bool Execute(const Heart::Action::FArguments& Arguments) PURE_VIRTUAL(UHeartGraphActionBase::Execute, return false; )
	virtual bool CanUndo(UObject* Target) const { return false; }
	virtual bool Undo(UObject* Target) { return false; }
};