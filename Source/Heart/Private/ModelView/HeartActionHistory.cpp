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
			FExecutingAction(const TSubclassOf<UHeartActionBase> Action, UHeartActionHistory* History, const FArguments& Arguments)
			  : Action(Action), History(History), Arguments(Arguments) {}
			TSubclassOf<UHeartActionBase> Action;
			UHeartActionHistory* History;
			const FArguments& Arguments;
			bool Cancelled = false;
		};

		/**
		 * While Actions execute within a single frame, it's possible for them to chain call other Actions, so a stack
		 * is needed to keep track of them.
		 * If an action is going to be logged, should it succeed, it will be stored here, otherwise a blank Option will
		 * fill its slot in the stack.
		 */
		static TArray<TOptional<FExecutingAction>> ExecutingActionsStack;

		void BeginLog(const UHeartActionBase* Action, const FArguments& Arguments)
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
						ExecutingActionsStack.Emplace(InPlace, Action->GetClass(), History, Arguments);
						return;
					}
				}
				// Unable to find the HeartGraph for some reason, so bail from trying to log.
			}

			// Store current action as non-loggable.
			ExecutingActionsStack.Emplace(FNullOpt(0));
		}

		void EndLog(const bool Successful, FBloodContainer* UndoData)
		{
			if (const TOptional<FExecutingAction> LoggedAction = ExecutingActionsStack.Pop();
				Successful && LoggedAction.IsSet())
			{
				const FExecutingAction& Value = LoggedAction.GetValue();

				// Skip if this action was canceled.
				if (Value.Cancelled)
				{
					return;
				}

				Value.History->AddRecord({Value.Action, Value.Arguments, MoveTemp(*UndoData)});
			}
		}
	}

	bool IsLoggable(const UHeartActionBase* Action, const FArguments& Arguments)
	{
		const bool ShouldLog = FNativeExec::CanUndo(Action, Arguments.Target) || EnumHasAnyFlags(Arguments.Flags, ForceRecord);
		const bool CannotLog = EnumHasAnyFlags(Arguments.Flags, DisallowRecord | IsRedo);

		return ShouldLog && !CannotLog;
	}

	bool IsLogging()
	{
		return !Impl::ExecutingActionsStack.IsEmpty() && Impl::ExecutingActionsStack.Last().IsSet();
	}

	bool IsUndoable()
	{
		// @note: Currently "Logging" and "Undoable" are the same, because we only log undoable actions. If that changed, update this.
		return IsLogging();
	}

	UHeartGraph* GetGraphFromActionStack()
	{
		return Impl::ExecutingActionsStack.Last()->History->GetGraph();
	}

	void CancelLog()
	{
		if (IsLogging())
		{
			Impl::ExecutingActionsStack.Last().GetValue().Cancelled = true;
		}
	}

	bool TryUndo(UHeartGraph* Graph)
	{
		if (!IsValid(Graph))
		{
			return false;
		}

		UHeartActionHistory* History = Graph->GetExtension<UHeartActionHistory>();
		if (!IsValid(History))
		{
			UE_LOG(LogHeartGraph, Warning, TEXT("Cannot perform Undo; Graph '%s' has no History extension!"), *Graph->GetName())
			return false;
		}

		return TryUndo(History);
	}

	bool TryUndo(UHeartActionHistory* History)
	{
		auto Record = History->RetrieveRecord();
		if (!Record.IsSet() ||
			!IsValid(Record->Action))
		{
			return false;
		}

		// Push an action frame
		Impl::ExecutingActionsStack.Emplace(InPlace, Record->Action, History, Record->Arguments);

		const bool Success = Undo(Record->Action, Record->Arguments.Target, Record->UndoData);

		// Pop the action frame
		Impl::ExecutingActionsStack.Pop();

		return Success;
	}

	FHeartEvent TryRedo(const UHeartGraph* Graph)
	{
		if (!IsValid(Graph))
		{
			return FHeartEvent::Failed;
		}

		UHeartActionHistory* History = Graph->GetExtension<UHeartActionHistory>();
		if (!IsValid(History))
		{
			UE_LOG(LogHeartGraph, Warning, TEXT("Cannot perform Undo; Graph '%s' has no History extension!"), *Graph->GetName())
			return FHeartEvent::Failed;
		}

		return History->Redo();
	}
}

bool FHeartActionRecord::Serialize(FArchive& Ar)
{
	Ar << Action << Arguments << UndoData;
	return true;
}

void UHeartActionHistory::AddRecord(const FHeartActionRecord& Record)
{
	// Clear history above Pointer
	Actions.SetNumUninitialized(ActionPointer + 1);

	// Remove the element at index 0, but don't shrink, because we are just going to add another element
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
	return Heart::Action::History::TryUndo(this);
}

FHeartEvent UHeartActionHistory::Redo()
{
	if (ActionPointer == Actions.Num()-1)
	{
		// Cannot Redo most recent action
		return FHeartEvent::Invalid;
	}

	// @todo it would be nice to not mutate the OriginalRecord's UndoData when Redo'ing, but not all actions are
	// completely compatible with reconstructing their state from undo data, example, creating new nodes always uses
	// a new guid, there is no API to construct a node with an existing guid.
	FHeartActionRecord& Original = Actions[++ActionPointer];
	FHeartActionRecord Copy = Original;
	Copy.Arguments.Activation = FHeartActionIsRedo{&Original.UndoData};
	EnumAddFlags(Copy.Arguments.Flags, Heart::Action::IsRedo);

	const UHeartActionBase* ActionObject = GetDefault<UHeartActionBase>(Original.Action);

	return Heart::Action::FNativeExec::Execute(ActionObject, Copy.Arguments);
}

bool UHeartActionHistory::IsUndoable()
{
	return Heart::Action::History::IsUndoable();
}