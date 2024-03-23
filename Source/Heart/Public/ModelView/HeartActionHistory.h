// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Actions/HeartGraphActionBase.h"
#include "Model/HeartGraphExtension.h"
#include "HeartActionHistory.generated.h"

namespace Heart::Action::History
{
	namespace Impl
	{
		HEART_API void BeginLog(UHeartGraphActionBase* Action, const FArguments& Arguments);
		HEART_API void EndLog(bool Success);
	}

	// Is this action being executing in a state that we want to record.
	HEART_API bool IsLoggable(const UHeartGraphActionBase* Action, const FArguments& Arguments);

	// Are we currently running an undoable action.
	HEART_API bool IsUndoable();

	// A simple wrapper around a lamba that executes an action. If the action is loggable, and it succeeds, it will be
	// recorded, provided that an Action History extension can be found.
	template <typename T>
	bool Log(UHeartGraphActionBase* Action, const FArguments& Arguments, T Lambda)
	{
		Impl::BeginLog(Action, Arguments);
		const bool Success = Lambda();
		Impl::EndLog(Success);
		return Success;
	}
}

USTRUCT()
struct FHeartActionRecord
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UHeartGraphActionBase> Action;

	// Original arguments used to execute this action. Used to 'Redo' an un-done action.
	Heart::Action::FArguments Arguments;
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
	bool Redo();

protected:
	// Are we currently running an undoable action.
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