// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasAction_AutoLayout.h"
#include "Model/HeartGraph.h"
#include "ModelView/HeartActionHistory.h"
#include "UMG/HeartGraphCanvas.h"
#include "ModelView/HeartLayoutHelper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasAction_AutoLayout)

static const FLazyName OriginalLocationsStorage("oldlocs");

FEventReply UHeartCanvasAction_AutoLayout::ExecuteOnGraph(UHeartGraphCanvas* CanvasGraph, const FHeartInputActivation& Activation,
														  UObject* ContextObject, FBloodContainer& UndoData) const
{
	const UHeartLayoutHelper* LayoutHelper = Cast<UHeartLayoutHelper>(ContextObject);

	if (!IsValid(LayoutHelper))
	{
		if (const TSubclassOf<UHeartLayoutHelper> LayoutClass = Cast<UClass>(ContextObject);
			IsValid(LayoutClass) && !LayoutClass->HasAnyClassFlags(CLASS_Abstract))
		{
			LayoutHelper = GetDefault<UHeartLayoutHelper>(LayoutClass);
		}
	}

	if (!IsValid(LayoutHelper))
	{
		UE_LOG(LogHeartGraphCanvas, Error, TEXT("UHeartCanvasAction_AutoLayout expects a context action that is a instance or class pointer of a UHeartLayoutHelper type."))
		return false;
	}

	if (Heart::Action::History::IsUndoable())
	{
		TMap<FHeartNodeGuid, FVector2D> OriginalLocations;

		CanvasGraph->GetGraph()->ForEachNode(
			[&](const UHeartGraphNode* Node)
			{
				OriginalLocations.Add(Node->GetGuid(), Node->GetLocation());
				return true;
			});

		UndoData.Add(OriginalLocationsStorage, OriginalLocations);
	}

	LayoutHelper->Layout(CanvasGraph);
	return true;
}

bool UHeartCanvasAction_AutoLayout::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	UHeartGraphCanvas* GraphCanvas = Cast<UHeartGraphCanvas>(Target);
	if (!IsValid(GraphCanvas))
	{
		return false;
	}

	UHeartGraph* Graph = GraphCanvas->GetHeartGraph();

	TSet<UHeartGraphNode*> Touched;

	auto&& Data = UndoData.Get<TMap<FHeartNodeGuid, FVector2D>>(OriginalLocationsStorage);

	for (auto&& OriginalLocation : Data)
	{
		Touched.Add(Graph->GetNode(OriginalLocation.Key));
		GraphCanvas->SetNodeLocation(OriginalLocation.Key, OriginalLocation.Value, false);
	}

	Graph->NotifyNodeLocationsChanged(Touched, false);

	return true;
}