// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Location/Actions/HeartAction_AutoLayout.h"
#include "Location/HeartLayoutHelper.h"
#include "Location/HeartNodeLocationInterface.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "ModelView/HeartActionHistory.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_AutoLayout)

static constexpr FLazyName OriginalLocationsStorage("oldlocs");

bool UHeartAction_AutoLayout::CanExecute(const UObject* Target) const
{
	return IsValid(Target) && Target->Implements<UHeartGraphInterface>();
}

FHeartEvent UHeartAction_AutoLayout::ExecuteOnGraph(UHeartGraph& Graph, const FHeartInputActivation& Activation,
													UObject* ContextObject, FBloodContainer& UndoData) const
{
	UHeartLayoutHelper* LayoutHelper = Cast<UHeartLayoutHelper>(ContextObject);

	if (!IsValid(LayoutHelper))
	{
		if (const TSubclassOf<UHeartLayoutHelper> LayoutClass = Cast<UClass>(ContextObject);
			IsValid(LayoutClass) && !LayoutClass->HasAnyClassFlags(CLASS_Abstract))
		{
			LayoutHelper = NewObject<UHeartLayoutHelper>(GetTransientPackage(), LayoutClass);
		}
	}

	if (!IsValid(LayoutHelper))
	{
		UE_LOG(LogHeartGraph, Error, TEXT("UHeartAction_AutoLayout expects a context action that is a instance or class pointer of a UHeartLayoutHelper type."))
		return FHeartEvent::Failed;
	}

	if (Heart::Action::History::IsUndoable())
	{
		TMap<FHeartNodeGuid, FVector2D> OriginalLocations;

		IHeartNodeLocationInterface* LocationInterface = Graph.GetNodeLocationInterface();

		Graph.ForEachNode(
			[&](const TPair<FHeartNodeGuid, UHeartGraphNode*>& Pair)
			{
				OriginalLocations.Add(Pair.Key, LocationInterface->GetNodeLocation(Pair.Key));
				return true;
			});

		UndoData.Add(OriginalLocationsStorage, OriginalLocations);
	}

	LayoutHelper->Layout(Graph, *Graph.GetNodeLocationInterface());
	return FHeartEvent::Handled;
}

bool UHeartAction_AutoLayout::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	UHeartGraph* Graph = Heart::Action::History::GetGraphFromActionStack();
	IHeartNodeLocationInterface* NodeLocationInterface = Graph->GetNodeLocationInterface();

	TSet<FHeartNodeGuid> Touched;

	auto&& Data = UndoData.Get<TMap<FHeartNodeGuid, FVector2D>>(OriginalLocationsStorage);

	for (auto&& OriginalLocation : Data)
	{
		Touched.Add(OriginalLocation.Key);
		NodeLocationInterface->SetNodeLocation(OriginalLocation.Key, OriginalLocation.Value, false);
	}

	Graph->NotifyNodeLocationsChanged(Touched, false);

	return true;
}