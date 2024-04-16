// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartMultiUndoHelper.h"
#include "Model/HeartGraph.h"
#include "ModelView/HeartActionHistory.h"
#include "ModelView/Actions/HeartAction_MultiUndoEnd.h"
#include "ModelView/Actions/HeartAction_MultiUndoStart.h"

namespace Heart::Action::History::MultiUndo
{
	void Start(IHeartGraphInterface* Graph)
	{
		check(Graph)
		UHeartGraph* HeartGraph = Graph->GetHeartGraph();
		if (!ensure(IsValid(HeartGraph)))
		{
			return;
		}

		if (UHeartActionHistory* History = HeartGraph->GetExtension<UHeartActionHistory>();
			!IsValid(History))
		{
			FHeartActionRecord Record;
			Record.Action = UHeartAction_MultiUndoStart::StaticClass();
			Record.Arguments.Target = HeartGraph;
			History->AddRecord(Record);
		}
	}

	void End(IHeartGraphInterface* Graph)
	{
		check(Graph)
		UHeartGraph* HeartGraph = Graph->GetHeartGraph();
		if (!ensure(IsValid(HeartGraph)))
		{
			return;
		}

		if (UHeartActionHistory* History = Graph->GetHeartGraph()->GetExtension<UHeartActionHistory>();
			!IsValid(History))
		{
			FHeartActionRecord Record;
			Record.Action = UHeartAction_MultiUndoEnd::StaticClass();
			Record.Arguments.Target = HeartGraph;
			History->AddRecord(Record);
		}
	}
}