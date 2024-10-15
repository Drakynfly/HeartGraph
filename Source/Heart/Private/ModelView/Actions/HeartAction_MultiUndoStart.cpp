// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_MultiUndoStart.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphInterface.h"
#include "ModelView/HeartActionHistory.h"
#include "ModelView/Actions/HeartAction_MultiUndoEnd.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_MultiUndoStart)

#define LOCTEXT_NAMESPACE "HeartAction_MultiUndoStart"

FText UHeartAction_MultiUndoStart::GetDescription(const UObject* Target) const
{
	return LOCTEXT("Description", "Multi-Undo [START]");
}

FHeartEvent UHeartAction_MultiUndoStart::Execute(const Heart::Action::FArguments& Arguments) const
{
	checkfSlow(Activation.IsRedoAction(), TEXT("UHeartAction_MultiUndoStart should only be executed as a Redo!"))

	auto Interface = Cast<IHeartGraphInterface>(Arguments.Target);
	if (!Interface)
	{
		return FHeartEvent::Invalid;
	}

	UHeartGraph* Graph = Interface->GetHeartGraph();
	if (!ensure(IsValid(Graph)))
	{
		return FHeartEvent::Invalid;
	}

	UHeartActionHistory* History = Graph->GetExtension<UHeartActionHistory>();
	if (!IsValid(History))
	{
		return FHeartEvent::Failed;
	}

	int32 ScopeCounter = 1;
	do
	{
		auto&& RecordView = History->AdvanceRecordPtr();
		if (!RecordView.IsValid())
		{
			// Reached the end of records to redo unexpectedly.
			UE_LOG(LogHeartGraph, Warning, TEXT("Redo of MultiUndo ran until most recent record without hitting an End!"))
			break;
		}

		auto&& Record = RecordView.Get();

		if (Record.Action == UHeartAction_MultiUndoStart::StaticClass())
		{
			// We hit another Start marker, so we will re-do everything in that scope as well.
			ScopeCounter++;
			continue;
		}

		if (Record.Action == UHeartAction_MultiUndoEnd::StaticClass())
		{
			// We hit an End marker, so we have re-done everything in this scope.
			ScopeCounter--;
			continue;
		}

		// If not a scope marker, redo the action.
		Heart::Action::History::RedoRecord(Record);
	}
	while (0 < ScopeCounter);

	return FHeartEvent::Handled;
}

#undef LOCTEXT_NAMESPACE