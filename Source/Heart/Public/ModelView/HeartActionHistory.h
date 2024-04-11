// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "BloodContainer.h"
#include "Input/HeartActionBase.h"
#include "Model/HeartGraphExtension.h"
#include "HeartActionHistory.generated.h"

class UHeartActionHistory;

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

	HEART_API bool TryUndo(UHeartGraph* Graph);
	HEART_API bool TryUndo(UHeartActionHistory* History);

	HEART_API FHeartEvent TryRedo(const UHeartGraph* Graph);
}

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

	bool Serialize(FArchive& Ar);
};

template<>
struct TStructOpsTypeTraits<FHeartActionRecord> : public TStructOpsTypeTraitsBase2<FHeartActionRecord>
{
	enum
	{
		WithSerializer = true,
	};
};

/**
 * A record of recent actions performed on a graph.
 */
UCLASS()
class HEART_API UHeartActionHistory : public UHeartGraphExtension
{
	GENERATED_BODY()

public:
	void AddRecord(const FHeartActionRecord& Record);

	TOptional<FHeartActionRecord> RetrieveRecord();
	TConstArrayView<FHeartActionRecord> RetrieveRecords(int32 Count);

	UFUNCTION(BlueprintCallable, Category = "Heart|ActionHistory")
	bool Undo();

	UFUNCTION(BlueprintCallable, Category = "Heart|ActionHistory")
	FHeartEvent Redo();

protected:
	// Are we currently running an undoable action?
	UFUNCTION(BlueprintPure, Category = "Heart|ActionHistory")
	static bool IsUndoable();


	/**		CONFIG		**/

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Heart|ActionHistory")
	int32 MaxRecordedActions = 50;

private:
	UPROPERTY()
	TArray<FHeartActionRecord> Actions;

	int32 ActionPointer = INDEX_NONE;
};