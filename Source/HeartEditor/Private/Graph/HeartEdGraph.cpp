// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartEdGraph.h"
#include "Graph/HeartEdGraphSchema.h"

#include "Model/HeartGraph.h"

#include "Kismet2/BlueprintEditorUtils.h"

UHeartEdGraph::UHeartEdGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UEdGraph* UHeartEdGraph::CreateGraph(UHeartGraph* InHeartGraph)
{
	UHeartEdGraph* NewGraph = CastChecked<UHeartEdGraph>(FBlueprintEditorUtils::CreateNewGraph(InHeartGraph, NAME_None, StaticClass(), UHeartEdGraphSchema::StaticClass()));
	NewGraph->bAllowDeletion = false;

	InHeartGraph->HeartEdGraph = NewGraph;
	NewGraph->GetSchema()->CreateDefaultNodesForGraph(*NewGraph);

	return NewGraph;
}

UHeartGraph* UHeartEdGraph::GetHeartGraph() const
{
	return CastChecked<UHeartGraph>(GetOuter());
}
