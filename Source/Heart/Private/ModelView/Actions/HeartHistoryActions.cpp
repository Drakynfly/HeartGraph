// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartHistoryActions.h"
#include "Model/HeartGraph.h"
#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartHistoryActions)

void UHeartUndoAction::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation,
									  UObject* ContextObject)
{
	UHeartActionHistory* History = Graph->GetExtension<UHeartActionHistory>();
	if (!IsValid(History))
	{
		UE_LOG(LogHeartGraph, Warning, TEXT("Cannot perform Undo; Graph '%s' has no History extension!"), *Graph->GetName())
		return;
	}

	History->Undo();
}

void UHeartRedoAction::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation,
	UObject* ContextObject)
{
	UHeartActionHistory* History = Graph->GetExtension<UHeartActionHistory>();
	if (!IsValid(History))
	{
		UE_LOG(LogHeartGraph, Warning, TEXT("Cannot perform Redo; Graph '%s' has no History extension!"), *Graph->GetName())
		return;
	}

	History->Redo();
}