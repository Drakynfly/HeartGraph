// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartEvent.h"
#include "Input/HeartInputActivation.h"

#include "HeartActionBase.generated.h"

struct FHeartManualEvent;
struct FBloodContainer;
class UHeartActionBase;

// This namespace is the c++ API for executing Heart Actions
namespace Heart::Action
{
	// System-level flags for optional behavior in actions
	enum EExecutionFlags
	{
		NoFlags = 0,

		// This action is being run as a Redo of a previously undone action
		IsRedo = 1 << 0,

		// Prevent this action from being recorded by action history
		DisallowRecord = 1 << 1,

		// Record this action, even if it doesn't have a reason to
		ForceRecord = 1 << 2,
	};

	struct FArguments
	{
		// Object to run an action on.
		UObject* Target = nullptr;

		// Input from the user or system that triggered this action.
		FHeartInputActivation Activation;

		// Optional payload object that execution can use if applicable.
		UObject* Payload = nullptr;

		// Behavior modification flags that can be checked during execution.
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

	// Direct access to the API of UHeartActionBase through here
	struct HEARTCORE_API FNativeExec
	{
		static FText GetDescription(const UHeartActionBase* Action, const UObject* Target);
		static bool CanExecute(const UHeartActionBase* Action, const UObject* Target);
		static FHeartEvent Execute(const UHeartActionBase* Action, const FArguments& Arguments);
		static bool CanUndo(const UHeartActionBase* Action, const UObject* Target);
		static bool Undo(const UHeartActionBase* Action, UObject* Target, const FBloodContainer& UndoData);
	};

	/** Gets the description for an action, optionally adaptive to a specific target */
	HEARTCORE_API FText GetDescription(const TSubclassOf<UHeartActionBase>& Action, const UObject* Target = nullptr);

	/** Can a Heart Action be run on a target? */
	HEARTCORE_API bool CanExecute(const TSubclassOf<UHeartActionBase>& Action, const UObject* Target);

	HEARTCORE_API FHeartEvent Execute(const TSubclassOf<UHeartActionBase>& Action, UObject* Target, const FHeartInputActivation& Activation, UObject* Payload = nullptr);

	HEARTCORE_API bool CanUndo(const TSubclassOf<UHeartActionBase>& Action, const UObject* Target);

	HEARTCORE_API bool Undo(const TSubclassOf<UHeartActionBase>& Action, UObject* Target, const FBloodContainer& UndoData);
}
ENUM_CLASS_FLAGS(Heart::Action::EExecutionFlags)


/**
 * Base class for scriptable actions that perform mutations on a Heart Graph or Heart Graph View.
 * Heart Actions are run by calling Execute on their CDO, so they do not need to be instanced, nor should they ever be.
 * All member functions are const, and all mutations to graph state are performed through Execution parameters.
 *
 * Any non-abstract child must implement `CanExecute` and `Execute` unless a parent does it for them.
 * Override GetDescription for a custom FText label used in user-facing UI.
 * Optionally, `CanUndo` & `Undo` may be implemented to support Undo/Redo behavior. If Undo is supported, then Redo is
 * implied to be supported as well. In the case of a Redo being run, Execute is called with valid UndoData.
 * This can be verified by calling `Activation.IsRedoAction()`
 */
UCLASS(Abstract, Const, meta = (DontUseGenericSpawnObject = "True"))
class HEARTCORE_API UHeartActionBase : public UObject
{
	GENERATED_BODY()

	friend struct Heart::Action::FNativeExec;

protected:
	//~ UObject
	virtual void PostInitProperties() override;
	//~ UObject


	/**		STATIC BLUEPRINT API	**/

	/** Gets the description for an action, optionally adaptive to a specific target */
	UFUNCTION(BlueprintPure, Category = "Heart|GraphActionBase")
	static FText GetActionDescription(TSubclassOf<UHeartActionBase> Class, const UObject* Target = nullptr);

	/** Can a Heart Action be run on a target? */
	UFUNCTION(BlueprintPure, Category = "Heart|GraphActionBase")
	static bool CanExecute(TSubclassOf<UHeartActionBase> Class, const UObject* Target);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase", meta = (AutoCreateRefTerm = "Activation"))
	static FHeartEvent ExecuteGraphAction(TSubclassOf<UHeartActionBase> Class, UObject* Target, const FHeartManualEvent& Activation);

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphActionBase", meta = (AutoCreateRefTerm = "Activation"))
	static FHeartEvent ExecuteGraphActionWithPayload(TSubclassOf<UHeartActionBase> Class, UObject* Target, const FHeartManualEvent& Activation, UObject* Payload);


	/**		VIRTUAL PROTECTED API	**/

	virtual FText GetDescription(const UObject* Target) const;

	virtual bool CanExecute(const UObject* Target) const PURE_VIRTUAL(UHeartActionBase::CanExecute, return false; )
	virtual FHeartEvent Execute(const Heart::Action::FArguments& Arguments) const
		PURE_VIRTUAL(UHeartActionBase::Execute, return FHeartEvent::Invalid; )

	// CanUndo and Undo must either both be implemented, or neither. If the action cannot be undone, leave CanUndo
	// un-overridden, as it returns false by default. If it is overridden to return true, then Undo will be called.
	virtual bool CanUndo(const UObject* Target) const { return false; }
	virtual bool Undo(UObject* Target, const FBloodContainer& UndoData) const
		PURE_VIRTUAL(UHeartActionBase::Undo, return false; )
};