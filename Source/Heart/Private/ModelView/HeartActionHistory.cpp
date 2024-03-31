// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/HeartActionHistory.h"
#include "Model/HeartGraphInterface.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphNodeInterface.h"
#include "Model/HeartGraphPinInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartActionHistory)

namespace Heart::Action::History
{
	namespace Impl
	{
		struct FExecutingAction
		{
			FExecutingAction(UHeartActionBase* Action, UHeartActionHistory* History, const FArguments& Arguments)
			  : Action(Action), History(History), Arguments(Arguments) {}
			UHeartActionBase* Action;
			UHeartActionHistory* History;
			const FArguments& Arguments;
		};

		/**
		 * While Actions execute within a single frame, it's possible for them to chain call other Actions, so a stack
		 * is needed to keep track of them.
		 * If an action is going to be logged, should it succeed, it will be stored here, otherwise a blank Option will
		 * fill its slot in the stack.
		 */
		static TArray<TOptional<FExecutingAction>> ExecutingActionsLoggableStack;

		void BeginLog(UHeartActionBase* Action, const FArguments& Arguments)
		{
			if (IsLoggable(Action, Arguments))
			{
				// Track down the graph from the target
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

				if (IsValid(HeartGraph))
				{
					if (UHeartActionHistory* History = HeartGraph->GetExtension<UHeartActionHistory>();
						IsValid(History))
					{
						// Store a log for the action
						ExecutingActionsLoggableStack.Emplace(InPlace, Action, History, Arguments);
						return;
					}
				}
				// Unable to find HeartGraph for some reason, so bail from trying to log.
			}

			// Store current action as non-loggable.
			ExecutingActionsLoggableStack.Emplace(FNullOpt(0));
		}

		void EndLog(const bool Success)
		{
			if (const TOptional<FExecutingAction> LoggedAction = ExecutingActionsLoggableStack.Pop();
				Success && LoggedAction.IsSet())
			{
				const FExecutingAction& Value = LoggedAction.GetValue();
				Value.History->AddRecord({Value.Action, Value.Arguments});
			}
		}
	}

	bool IsLoggable(const UHeartActionBase* Action, const FArguments& Arguments)
	{
		const bool ShouldLog = Action->CanUndo(Arguments.Target) || EnumHasAnyFlags(Arguments.Flags, ForceRecord);
		const bool CannotLog = EnumHasAnyFlags(Arguments.Flags, DisallowRecord | IsRedo);

		return ShouldLog && !CannotLog;
	}

	bool IsUndoable()
	{
		return !Impl::ExecutingActionsLoggableStack.IsEmpty() && Impl::ExecutingActionsLoggableStack.Last().IsSet();
	}
}

bool FHeartActionRecord::Serialize(FArchive& Ar)
{
	// @todo test this works

	Ar << Action;
	Ar << Arguments.Target;

	FHeartInputActivation::StaticStruct()->GetCppStructOps()->Serialize(Ar, &Arguments.Activation);

	Ar << Arguments.Payload;
	Ar << *reinterpret_cast<uint8*>(&Arguments.Flags);

	return true;
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

bool UHeartActionHistory::IsUndoable()
{
	return Heart::Action::History::IsUndoable();
}