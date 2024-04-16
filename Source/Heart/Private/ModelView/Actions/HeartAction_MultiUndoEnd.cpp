// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_MultiUndoEnd.h"
#include "Model/HeartGraph.h"
#include "ModelView/HeartActionHistory.h"
#include "ModelView/Actions/HeartAction_MultiUndoStart.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_MultiUndoEnd)

#define LOCTEXT_NAMESPACE "HeartAction_MultiUndoEnd"

FText UHeartAction_MultiUndoEnd::GetDescription(const UObject* Target) const
{
	return LOCTEXT("Description", "Multi-Undo [END]");
}

bool UHeartAction_MultiUndoEnd::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	auto Interface = Cast<IHeartGraphInterface>(Target);
	if (!Interface)
	{
		return false;
	}

	UHeartGraph* Graph = Interface->GetHeartGraph();
	if (!ensure(IsValid(Graph)))
	{
		return false;
	}

	UHeartActionHistory* History = Graph->GetExtension<UHeartActionHistory>();
	if (!IsValid(History))
	{
		return false;
	}

	int32 ScopeCounter = 1;
	do
	{
		auto&& Record = History->RetrieveRecordPtr();
		if (!Record)
		{
			// Reached end of records to undo unexpectedly.
			UE_LOG(LogHeartGraph, Warning, TEXT("Undo of MultiUndo ran until most recent record without hitting an End!"))
			break;
		}

		if (Record->Action == UHeartAction_MultiUndoEnd::StaticClass())
		{
			// We hit another End marker, so we will undo everything in that scope as well.
			ScopeCounter++;
			continue;
		}

		if (Record->Action == UHeartAction_MultiUndoStart::StaticClass())
		{
			// We hit a Start marker, so we have undone everything in this scope.
			ScopeCounter--;
			continue;
		}

		// If not a scope marker, undo the action.
		Heart::Action::History::UndoRecord(*Record, History);
	}
	while (0 < ScopeCounter);

	return true;
}

#undef LOCTEXT_NAMESPACE