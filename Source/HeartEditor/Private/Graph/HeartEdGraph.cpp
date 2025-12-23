// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartEdGraph.h"
#include "Editor.h"
#include "HeartEditorShared.h"

#include "HeartRegistryEditorSubsystem.h"
#include "Engine/Engine.h"
#include "Graph/HeartEdGraphSchema.h"
#include "Input/HeartInputBindingAsset.h"
#include "Input/HeartInputHandler_Action.h"
#include "Input/HeartInputLinkerBase.h"
#include "Input/HeartInputTrigger.h"
#include "Input/HeartSlateInputLinker.h"

#include "Model/HeartGraph.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "Model/HeartGraphNode.h"
#include "ModelView/HeartGraphSchema.h"
#include "Nodes/HeartEdGraphNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartEdGraph)

static TAutoConsoleVariable<bool> CVarCreateDebugLinkerAction(
	TEXT("Heart.CreateDebugLinkerAction"),
	false,
	TEXT("Create a basic Heart Action called \"Debug Action\" in new HeartEdGraphs.")
);

FText UHeartEditorDebugAction::GetDescription(const UObject* Target) const
{
	return FText::FromStringView(TEXTVIEW("Debug Action"));
}

bool UHeartEditorDebugAction::CanExecute(const UObject* Target) const
{
	return IsValid(Target);
}

FHeartEvent UHeartEditorDebugAction::ExecuteOnGraph(UHeartGraph& Graph, const FHeartInputActivation& Activation,
													UObject* ContextObject, FBloodContainer& UndoData) const
{
	GEngine->AddOnScreenDebugMessage(uint64(this), 10.f, Heart::EditorShared::HeartColor.ToFColor(true),
		FString::Printf(TEXT("Executing Debug Action on graph '%s'"), *Graph.GetName()));
	return FHeartEvent::Handled;
}

FHeartEvent UHeartEditorDebugAction::ExecuteOnNode(UHeartGraph& Graph, const FHeartNodeGuid& Node, const FHeartInputActivation& Activation,
												   UObject* ContextObject, FBloodContainer& UndoData) const
{
	GEngine->AddOnScreenDebugMessage(uint64(this), 10.f, Heart::EditorShared::HeartColor.ToFColor(true),
	FString::Printf(TEXT("Executing Debug Action on node '%s'"), *Node.ToString()));
	return FHeartEvent::Handled;
}

FHeartEvent UHeartEditorDebugAction::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin,
												  const FHeartInputActivation& Activation, UObject* ContextObject, FBloodContainer& UndoData) const
{
	GEngine->AddOnScreenDebugMessage(uint64(this), 10.f, Heart::EditorShared::HeartColor.ToFColor(true),
	FString::Printf(TEXT("Executing Debug Action on pin '%s'"), Pin ? *Pin->GetPinGuid().ToString() : TEXT("null")));
	return FHeartEvent::Handled;
}

UHeartEdGraph::UHeartEdGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHeartEdGraph::PostInitProperties()
{
	Super::PostInitProperties();

	if (!IsTemplate())
	{
		UHeartGraph* HeartGraph = GetHeartGraph_Implementation();

		HeartGraph->GetOnNodeAddOrRemove().AddUObject(this, &ThisClass::OnNodeAddedOrRemoved);
		HeartGraph->GetOnNodeConnectionsChanged().AddUObject(this, &ThisClass::OnNodeConnectionsChanged);
	}
}

void UHeartEdGraph::PostLoad()
{
	Super::PostLoad();

	const UHeartGraph* HeartGraph = GetHeartGraph_Implementation();

	for (auto&& Element : HeartGraph->Nodes)
	{
		if (IsValid(Element.Value))
		{
			// For various reasons, runtime nodes could be missing a EdGraph equivalent, and we want to silently repair these,
			// or these nodes will be invisible in the EdGraph
			if (!IsValid(FindEdGraphNodeForNode(Element.Key)))
			{
				CreateEdGraphNode(Element.Value);
			}
		}
	}

	//CreateSlateInputLinker();
}

UEdGraph* UHeartEdGraph::CreateGraph(UHeartGraph* InHeartGraph)
{
	UHeartEdGraph* NewEdGraph = CastChecked<UHeartEdGraph>(FBlueprintEditorUtils::CreateNewGraph(InHeartGraph, NAME_None, StaticClass(), UHeartEdGraphSchema::StaticClass()));
	NewEdGraph->bAllowDeletion = false;

	InHeartGraph->HeartEdGraph = NewEdGraph;
	for (auto&& Element : InHeartGraph->Nodes)
	{
		if (IsValid(Element.Value))
		{
			NewEdGraph->CreateEdGraphNode(Element.Value);
		}
	}

	NewEdGraph->CreateSlateInputLinker();

	return NewEdGraph;
}

UHeartEdGraphNode* UHeartEdGraph::FindEdGraphNode(const TFunction<bool(const UHeartEdGraphNode*)>& Iter)
{
	for (auto Element : Nodes)
	{
		if (UHeartEdGraphNode* HeartEdGraphNode = Cast<UHeartEdGraphNode>(Element))
		{
			if (Iter(HeartEdGraphNode))
			{
				return HeartEdGraphNode;
			}
		}
	}
	return nullptr;
}

UHeartEdGraphNode* UHeartEdGraph::FindEdGraphNodeForNode(const FHeartNodeGuid& HeartNode)
{
	return FindEdGraphNode(
		[HeartNode](const UHeartEdGraphNode* Node)
		{
			return Node->GetNodeGuid() == HeartNode;
		});
}

UHeartGraph* UHeartEdGraph::GetHeartGraph_Implementation() const
{
	return CastChecked<UHeartGraph>(GetOuter());
}

UHeartSlateInputLinker* UHeartEdGraph::GetEditorLinker() const
{
	if (!IsValid(SlateInputLinker))
	{
		// @todo why
		const_cast<ThisClass*>(this)->CreateSlateInputLinker();
	}

	return SlateInputLinker;
}

void UHeartEdGraph::CreateSlateInputLinker()
{
	UHeartGraph* Graph = GetHeartGraph_Implementation();
	if (!IsValid(Graph))
	{
		return;
	}

	auto&& RuntimeSchema = Graph->GetSchema();

	auto InputLinkerClass = RuntimeSchema->GetEditorLinkerClass();
	if (!IsValid(InputLinkerClass))
	{
		InputLinkerClass = UHeartSlateInputLinker::StaticClass();
	}

	SlateInputLinker = NewObject<UHeartSlateInputLinker>(this, InputLinkerClass);

	if (CVarCreateDebugLinkerAction.GetValueOnGameThread())
	{
		FHeartBoundInput DebugInput;

		// Create debug trigger
		FHeartInputTrigger_Manual Trigger;
		Trigger.Keys.Add(FName(TEXT("A")));
		DebugInput.Triggers.Add(FInstancedStruct::Make(Trigger));

		// Create debug action
		UHeartInputHandler_Action* DebugAction = NewObject<UHeartInputHandler_Action>();
		DebugAction->SetAction(UHeartEditorDebugAction::StaticClass());
		DebugInput.InputHandler = DebugAction;

		SlateInputLinker->AddBindings({DebugInput});
	}
}

void UHeartEdGraph::CreateEdGraphNode(UHeartGraphNode* Node)
{
	Modify();

	auto&& HeartGraph = GetHeartGraph_Implementation();
	HeartGraph->Modify();

	if (!ensure(GEditor)) return;

	const TSubclassOf<UHeartEdGraphNode> EdGraphNodeClass = GEditor->GetEditorSubsystem<UHeartRegistryEditorSubsystem>()->GetAssignedEdGraphNodeClass(Node->GetClass());

	FGraphNodeCreator<UHeartEdGraphNode> HeartNodeCreator(*this);

	auto&& NewEdGraphNode = HeartNodeCreator.CreateNode(false, EdGraphNodeClass);

	const FVector2D Location = Heart::Features::Location::GetNodeLocation(*Node);
	NewEdGraphNode->NodePosX = static_cast<int32>(Location.X);
	NewEdGraphNode->NodePosY = static_cast<int32>(Location.Y);

	// Assign runtime node pointer
	NewEdGraphNode->SetHeartGraphNode(Node);

	HeartNodeCreator.Finalize();

	// Since we live in the HeartGraph, mark it as having changed.
	HeartGraph->PostEditChange();
	(void)HeartGraph->MarkPackageDirty();
}

void UHeartEdGraph::OnNodeAddedOrRemoved(const FHeartNodeAddOrRemoveEvent& Event)
{
	switch (Event.Type)
	{
	case EHeartNodeAddOrRemoveEventType::Add:
		{
			UHeartGraph* Graph = GetHeartGraph_Implementation();
			for (auto&& Node : Event.Nodes)
			{
				if (UHeartGraphNode* GraphNode = Graph->GetNode(Node);
					IsValid(GraphNode))
				{
					if (auto&& EdGraphNode = FindEdGraphNodeForNode(Node))
					{
						AddNode(EdGraphNode);
					}
					else
					{
						CreateEdGraphNode(GraphNode);
					}
				}
			}
		}
		break;
	case EHeartNodeAddOrRemoveEventType::Remove:
		{
			for (auto&& Node : Event.Nodes)
			{
				if (auto&& EdGraphNode = FindEdGraphNodeForNode(Node))
				{
					EdGraphNode->DestroyNode();
				}
			}
		}
		break;
	}
}

void UHeartEdGraph::OnNodeConnectionsChanged(const FHeartGraphConnectionEvent& HeartGraphConnectionEvent)
{
	// @todo this needs to be more robust, and not break with bulk ConnectionEvents

	if (HeartGraphConnectionEvent.AffectedNodes.Num() != 2 ||
		HeartGraphConnectionEvent.AffectedPins.Num() != 2)
	{
		return;
	}

	const UHeartGraphNode* NodeA = HeartGraphConnectionEvent.AffectedNodes.Get(FSetElementId::FromInteger(0));
	const UHeartGraphNode* NodeB = HeartGraphConnectionEvent.AffectedNodes.Get(FSetElementId::FromInteger(1));
	const UEdGraphNode* EdNodeA = FindEdGraphNodeForNode(NodeA->GetGuid());
	const UEdGraphNode* EdNodeB = FindEdGraphNodeForNode(NodeB->GetGuid());
	const FHeartPinGuid PinAGuid = HeartGraphConnectionEvent.AffectedPins.Get(FSetElementId::FromInteger(0));
	const FHeartPinGuid PinBGuid = HeartGraphConnectionEvent.AffectedPins.Get(FSetElementId::FromInteger(1));

	if (EdNodeA && EdNodeB)
	{
		auto&& EdGraphPinA = EdNodeA->FindPin(NodeA->GetPinDescChecked(PinAGuid).Name);
		auto&& EdGraphPinB = EdNodeB->FindPin(NodeB->GetPinDescChecked(PinBGuid).Name);

		if (EdGraphPinA && EdGraphPinB)
		{
			auto NodeAView = NodeA->ViewConnections(PinAGuid);
			auto NodeBView = NodeB->ViewConnections(PinBGuid);

			if (NodeAView.IsValid() && NodeBView.IsValid() &&
				NodeAView.Get().GetLinks().Contains(NodeB->GetPinReference(PinBGuid)) &&
				NodeBView.Get().GetLinks().Contains(NodeA->GetPinReference(PinAGuid)))
			{
				EdGraphPinA->MakeLinkTo(EdGraphPinB);
			}
			else
			{
				EdGraphPinA->BreakLinkTo(EdGraphPinB);
			}
		}
	}
}