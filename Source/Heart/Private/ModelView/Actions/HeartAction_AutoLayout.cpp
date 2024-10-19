// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "ModelView/Actions/HeartAction_AutoLayout.h"
#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "ModelView/HeartActionHistory.h"
#include "ModelView/HeartLayoutHelper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartAction_AutoLayout)

static const FLazyName OriginalLocationsStorage("oldlocs");

bool UHeartAction_AutoLayout::CanExecute(const UObject* Target) const
{
	return IsValid(Target) && Target->Implements<UHeartGraphInterface>();
}

FHeartEvent UHeartAction_AutoLayout::ExecuteOnGraph(IHeartGraphInterface* Graph, const FHeartInputActivation& Activation,
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

		Graph->GetHeartGraph()->ForEachNode(
			[&](const UHeartGraphNode* Node)
			{
				OriginalLocations.Add(Node->GetGuid(), Node->GetLocation());
				return true;
			});

		UndoData.Add(OriginalLocationsStorage, OriginalLocations);
	}

	LayoutHelper->Layout(Graph);
	return FHeartEvent::Handled;
}

bool UHeartAction_AutoLayout::Undo(UObject* Target, const FBloodContainer& UndoData) const
{
	// We must use the Interface for the SetNodeLocation calls, in case it's overriden.
	IHeartGraphInterface* Interface = Cast<IHeartGraphInterface>(Target);

	TSet<FHeartNodeGuid> Touched;

	auto&& Data = UndoData.Get<TMap<FHeartNodeGuid, FVector2D>>(OriginalLocationsStorage);

	for (auto&& OriginalLocation : Data)
	{
		Touched.Add(OriginalLocation.Key);
		Interface->SetNodeLocation(OriginalLocation.Key, OriginalLocation.Value, false);
	}

	Interface->GetHeartGraph()->NotifyNodeLocationsChanged(Touched, false);

	return true;
}