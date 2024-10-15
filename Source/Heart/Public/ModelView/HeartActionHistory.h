// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "BloodContainer.h"
#include "Input/HeartActionBase.h"
#include "Model/HeartGraphExtension.h"
#include "StructUtils/StructView.h"
#include "HeartActionHistory.generated.h"

class UHeartActionHistory;

USTRUCT(BlueprintType, meta = (HasNativeBreak = "/Script/Heart.HeartGraphUtils.BreakHeartActionRecord"))
struct FHeartActionRecord
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UHeartActionBase> Action;

	// Original arguments used to execute this action. Used to 'Redo' an undone action.
	Heart::Action::FArguments Arguments;

	// Data stored by the action when it first ran, such as mouse position or node guids, everything needed to undo the action.
	UPROPERTY()
	FBloodContainer UndoData;

	bool Serialize(FArchive& Ar)
	{
		Ar << Action << Arguments << UndoData;
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FHeartActionRecord> : public TStructOpsTypeTraitsBase2<FHeartActionRecord>
{
	enum
	{
		WithSerializer = true,
	};
};

namespace Heart::Action::History
{
	namespace Impl
	{
		HEART_API void BeginLog(const UHeartActionBase* Action, const FArguments& Arguments);
		HEART_API void EndLog(bool Successful, FBloodContainer* UndoData);
	}

	// Is this action being executing in a state that we want to record?
	HEART_API bool IsLoggable(const UHeartActionBase* Action, const FArguments& Arguments);

	// Are we currently running an undoable action?
	HEART_API bool IsUndoable();

	// Callable while inside an Execute or Undo callstack; retrieves the owning Graph for the History Extension on top
	HEART_API UHeartGraph* GetGraphFromActionStack();

	// A simple wrapper around a lambda that executes an action. If the action is loggable, and it succeeds, it will be
	// recorded if an Action History extension can be found.
	template <typename T>
	FHeartEvent Log(const UHeartActionBase* Action, const FArguments& Arguments, T Lambda)
	{
		Impl::BeginLog(Action, Arguments);
		FHeartEvent Event;
		if (EnumHasAnyFlags(Arguments.Flags, IsRedo))
		{
			Event = Lambda(*Arguments.Activation.As<FHeartActionIsRedo>()->UndoneData);
			Impl::EndLog(Event.WasEventSuccessful(), nullptr);
		}
		else
		{
			FBloodContainer NewUndoData;
			Event = Lambda(NewUndoData);
			Impl::EndLog(Event.WasEventSuccessful(), &NewUndoData);
		}
		return Event;
	}

	// Cancel the executing logging context. The currently running action will not be logged.
	HEART_API void CancelLog();

	HEART_API bool UndoRecord(const FHeartActionRecord& Record, UHeartActionHistory* History);
	HEART_API FHeartEvent RedoRecord(const FHeartActionRecord& Record);

	HEART_API bool TryUndo(const UHeartGraph* Graph);
	HEART_API bool TryUndo(UHeartActionHistory* History);

	HEART_API FHeartEvent TryRedo(const UHeartGraph* Graph);
	HEART_API FHeartEvent TryRedo(UHeartActionHistory* History);
}


using FHeartActionHistoryRecordUpdate = TMulticastDelegate<void(int32, int32)>;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHeartActionHistoryRecordUpdate_BP, int32, Index, int32, Count);

using FHeartActionHistoryPointerChanged = TMulticastDelegate<void(int32)>;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeartActionHistoryPointerChanged_BP, int32, Pointer);

/**
 * A record of recent actions performed on a graph.
 */
UCLASS()
class HEART_API UHeartActionHistory : public UHeartGraphExtension
{
	GENERATED_BODY()

public:
	void AddRecord(const FHeartActionRecord& Record);

	TConstStructView<FHeartActionRecord> RetrieveRecordPtr();
	TConstStructView<FHeartActionRecord> AdvanceRecordPtr();

	// Grab the most recent N records and rewind the ActionPointer to behind them.
	TConstArrayView<FHeartActionRecord> RetrieveRecords(int32 Count);

	// Grab the most recent N records without changing the ActionPointer.
	TConstArrayView<FHeartActionRecord> ViewRecords(int32 Count) const;

	FHeartActionHistoryRecordUpdate& GetOnRecordsUpdated() { return OnRecordsUpdatedNative; }
	FHeartActionHistoryPointerChanged& GetOnPointerChanged() { return OnPointerChangedNative; }

	UFUNCTION(BlueprintCallable, Category = "Heart|ActionHistory")
	int32 GetActionPointer() const { return ActionPointer; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|ActionHistory")
	void ViewRecords(int32 Count, TArray<FHeartActionRecord>& Records) const;

	UFUNCTION(BlueprintCallable, BlueprintSetter, Category = "Heart|ActionHistory")
	void SetMaxRecordedActions(int32 Count);

	UFUNCTION(BlueprintCallable, Category = "Heart|ActionHistory")
	bool Undo();

	UFUNCTION(BlueprintCallable, Category = "Heart|ActionHistory")
	FHeartEvent Redo();

private:
	void BroadcastPointer();
	void BroadcastUpdate(int32 Index, int32 Count);

protected:
	FHeartActionHistoryRecordUpdate OnRecordsUpdatedNative;
	FHeartActionHistoryPointerChanged OnPointerChangedNative;

	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FHeartActionHistoryRecordUpdate_BP OnRecordsUpdated;

	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FHeartActionHistoryPointerChanged_BP OnPointerChanged;

	// Are we currently running an undoable action?
	UFUNCTION(BlueprintPure, Category = "Heart|ActionHistory")
	static bool IsUndoable();


	/**		CONFIG		**/

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Heart|ActionHistory")
	int32 MaxRecordedActions = 50;


	/**		STATE		**/
private:
	// Index of the most recently ran action. This means that ActionPointer will be equal to Actions.Num()-1 after running
	// a new action. Undoing actions will decrement this to the index of the undone action.
	UPROPERTY()
	int32 ActionPointer = INDEX_NONE;

	UPROPERTY()
	TArray<FHeartActionRecord> Actions;
};