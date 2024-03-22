// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartActionHistory.h"
#include "Model/HeartGraphInterface.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNodeInterface.h"
#include "Model/HeartGraphPinInterface.h"

void UHeartActionHistory::TryLogAction(UHeartGraphActionBase* Action, const Heart::Action::FArguments& Arguments)
{
	check(Action);

	const bool ShouldLog = Action->CanUndo(Arguments.Target) || EnumHasAnyFlags(Arguments.Flags, Heart::Action::ForceRecord);
	const bool CannotLog = EnumHasAnyFlags(Arguments.Flags, Heart::Action::DisallowRecord | Heart::Action::IsRedo);

	if (ShouldLog && !CannotLog)
	{
		const UHeartGraph* HeartGraph = nullptr;

		if (Arguments.Target->Implements<UHeartGraphInterface>())
		{
			HeartGraph = Cast<IHeartGraphInterface>(Arguments.Target)->GetHeartGraph();
		}
		else if (Arguments.Target->Implements<UHeartGraphNodeInterface>())
		{
			HeartGraph = Cast<IHeartGraphNodeInterface>(Arguments.Target)->GetHeartGraphNode()->GetGraph();
		}
		else if (Arguments.Target->Implements<UHeartGraphPinInterface>())
		{
			HeartGraph = Cast<IHeartGraphPinInterface>(Arguments.Target)->GetHeartGraphNode()->GetGraph();
		}

		if (!IsValid(HeartGraph))
		{
			return;
		}

		if (const auto History = HeartGraph->GetExtension<ThisClass>())
		{
			History->AddRecord({Action, Arguments});
		}
	}
}

void UHeartActionHistory::AddRecord(const FHeartActionRecord& Record)
{
	// Clear history above Pointer
	Actions.SetNumUninitialized(ActionPointer + 1);

	// Remove element at index 0, but don't shrink array, because we are just going to add another element
	if (Actions.Num() == MaxRecordedActions)
	{
		Actions.RemoveAt(0, 1, false);
	}

	// Add action to record, and reassign Pointer to new index
	ActionPointer = Actions.Emplace(Record);
}

TOptional<FHeartActionRecord> UHeartActionHistory::RetrieveRecord()
{
	if (ActionPointer == INDEX_NONE)
	{
		return {};
	}
	return Actions[ActionPointer--];
}

TConstArrayView<FHeartActionRecord> UHeartActionHistory::RetrieveRecords(const int32 Count)
{
	if (Count >= Actions.Num())
	{
		ActionPointer = INDEX_NONE;
		return MakeArrayView(Actions);
	}

	ActionPointer -= Count;
	return MakeArrayView(Actions).Right(Count);
}

bool UHeartActionHistory::Undo()
{
	auto Record = RetrieveRecord();
	if (!Record.IsSet() ||
		!IsValid(Record->Action))
	{
		return false;
	}

	return Record->Action->Undo(Record->Arguments.Target);
}

bool UHeartActionHistory::Redo()
{
	if (ActionPointer == Actions.Num()-1)
	{
		// Cannot Redo most recent action
		return false;
	}

	auto Record = Actions[++ActionPointer];
	Record.Arguments.Activation = FHeartActionIsRedo();
	EnumAddFlags(Record.Arguments.Flags, Heart::Action::IsRedo);
	return Record.Action->Execute(Record.Arguments);
}