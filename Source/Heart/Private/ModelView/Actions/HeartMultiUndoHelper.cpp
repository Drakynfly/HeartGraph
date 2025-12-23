// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartMultiUndoHelper.h"
#include "Model/HeartGraph.h"
#include "ModelView/HeartActionHistory.h"
#include "ModelView/Actions/HeartAction_MultiUndoEnd.h"
#include "ModelView/Actions/HeartAction_MultiUndoStart.h"

namespace Heart::Action::History::MultiUndo
{
	void Start(UHeartGraph& Graph)
	{
		if (UHeartActionHistory* History = Graph.GetExtension<UHeartActionHistory>();
			IsValid(History))
		{
			FHeartActionRecord Record;
			Record.Action = UHeartAction_MultiUndoStart::StaticClass();
			Record.Arguments.Target = &Graph;
			History->AddRecord(Record);
		}
		else
		{
			UE_LOG(LogHeartGraph, Error, TEXT("Unable to Start MultiUndo, no History Extension found on graph!"))
		}
	}

	void End(UHeartGraph& Graph)
	{
		if (UHeartActionHistory* History = Graph.GetExtension<UHeartActionHistory>();
			IsValid(History))
		{
			FHeartActionRecord Record;
			Record.Action = UHeartAction_MultiUndoEnd::StaticClass();
			Record.Arguments.Target = &Graph;
			History->AddRecord(Record);
		}
		else
		{
			UE_LOG(LogHeartGraph, Error, TEXT("Unable to End MultiUndo, no History Extension found on graph!"))
		}
	}
}