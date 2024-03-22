// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Actions/HeartGraphActionBase.h"
#include "Model/HeartGraphExtension.h"
#include "HeartActionHistory.generated.h"

class UHeartGraphActionBase;

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
	static void TryLogAction(UHeartGraphActionBase* Action, const Heart::Action::FArguments& Arguments);

	void AddRecord(const FHeartActionRecord& Record);

	TOptional<FHeartActionRecord> RetrieveRecord();
	TConstArrayView<FHeartActionRecord> RetrieveRecords(int32 Count);

	UFUNCTION(BlueprintCallable, Category = "Heart|ActionHistory")
	bool Undo();

	UFUNCTION(BlueprintCallable, Category = "Heart|ActionHistory")
	bool Redo();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Heart|ActionHistory")
	int32 MaxRecordedActions = 50;

private:
	UPROPERTY()
	TArray<FHeartActionRecord> Actions;

	int32 ActionPointer = INDEX_NONE;
};