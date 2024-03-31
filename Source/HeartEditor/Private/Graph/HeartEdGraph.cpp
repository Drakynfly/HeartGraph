// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartEdGraph.h"
#include "HeartEditorShared.h"

#include "HeartRegistryEditorSubsystem.h"
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

FText UHeartEditorDebugAction::GetDescription(const UObject* Target) const
{
	return FText::FromStringView(TEXTVIEW("Debug Action"));
}

bool UHeartEditorDebugAction::CanExecute(const UObject* Target) const
{
	return IsValid(Target);
}

void UHeartEditorDebugAction::ExecuteOnGraph(UHeartGraph* Graph, const FHeartInputActivation& Activation,
											 UObject* ContextObject)
{
	GEngine->AddOnScreenDebugMessage(uint64(this), 10.f, Heart::EditorShared::HeartColor.ToFColor(true),
		FString::Printf(TEXT("Executing Debug Action on graph '%s'"), Graph ? *Graph->GetName() : TEXT("null")));
}

void UHeartEditorDebugAction::ExecuteOnNode(UHeartGraphNode* Node, const FHeartInputActivation& Activation,
	UObject* ContextObject)
{
	GEngine->AddOnScreenDebugMessage(uint64(this), 10.f, Heart::EditorShared::HeartColor.ToFColor(true),
	FString::Printf(TEXT("Executing Debug Action on node '%s'"), Node ? *Node->GetName() : TEXT("null")));
}

void UHeartEditorDebugAction::ExecuteOnPin(const TScriptInterface<IHeartGraphPinInterface>& Pin,
	const FHeartInputActivation& Activation, UObject* ContextObject)
{
	GEngine->AddOnScreenDebugMessage(uint64(this), 10.f, Heart::EditorShared::HeartColor.ToFColor(true),
	FString::Printf(TEXT("Executing Debug Action on pin '%s'"), Pin ? *Pin.GetObject()->GetName() : TEXT("null")));
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
		UHeartGraph* HeartGraph = GetHeartGraph();

		HeartGraph->OnNodeAdded.AddUObject(this, &ThisClass::OnNodeAdded);
		HeartGraph->OnNodeRemoved.AddUObject(this, &ThisClass::OnNodeRemoved);
		HeartGraph->OnNodeConnectionsChanged.AddUObject(this, &ThisClass::OnNodeConnectionsChanged);
	}
}

void UHeartEdGraph::PostLoad()
{
	Super::PostLoad();

	const UHeartGraph* HeartGraph = GetHeartGraph();

	for (auto&& Element : HeartGraph->Nodes)
	{
		if (IsValid(Element.Value))
		{
			// For various reasons, runtime nodes could be missing a EdGraph equivalent, and we want to silently repair these,
			// or these nodes will be invisible in the EdGraph
			if (!IsValid(FindEdGraphNodeForNode(Element.Value)))
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

	NewEdGraph->GetSchema()->CreateDefaultNodesForGraph(*NewEdGraph);

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

UHeartEdGraphNode* UHeartEdGraph::FindEdGraphNodeForNode(const UHeartGraphNode* HeartGraphNode)
{
	return FindEdGraphNode(
		[HeartGraphNode](const UHeartEdGraphNode* Node)
		{
			return Node->GetHeartGraphNode() == HeartGraphNode;
		});
}

UHeartGraph* UHeartEdGraph::GetHeartGraph() const
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
	if (!IsValid(GetHeartGraph()))
	{
		return;
	}

	auto&& RuntimeSchema = GetHeartGraph()->GetSchema();

	auto InputLinkerClass = RuntimeSchema->GetEditorLinkerClass();
	if (!IsValid(InputLinkerClass))
	{
		InputLinkerClass = UHeartSlateInputLinker::StaticClass();
	}

	SlateInputLinker = NewObject<UHeartSlateInputLinker>(this, InputLinkerClass);

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

void UHeartEdGraph::CreateEdGraphNode(UHeartGraphNode* Node)
{
	Modify();

	auto&& HeartGraph = GetHeartGraph();
	HeartGraph->Modify();

	if (!ensure(GEditor)) return;

	const TSubclassOf<UHeartEdGraphNode> EdGraphNodeClass = GEditor->GetEditorSubsystem<UHeartRegistryEditorSubsystem>()->GetAssignedEdGraphNodeClass(Node->GetClass());

	FGraphNodeCreator<UHeartEdGraphNode> HeartNodeCreator(*this);

	auto&& NewEdGraphNode = HeartNodeCreator.CreateNode(false, EdGraphNodeClass);

	NewEdGraphNode->NodePosX = Node->GetLocation().X;
	NewEdGraphNode->NodePosY = Node->GetLocation().Y;

	// Assign runtime node pointer
	NewEdGraphNode->SetHeartGraphNode(Node);

	HeartNodeCreator.Finalize();

	// Since we live in the HeartGraph, mark it as having changed.
	HeartGraph->PostEditChange();
	HeartGraph->MarkPackageDirty();
}

void UHeartEdGraph::OnNodeAdded(UHeartGraphNode* HeartGraphNode)
{
	if (IsValid(HeartGraphNode))
	{
		if (auto&& EdGraphNode = FindEdGraphNodeForNode(HeartGraphNode))
		{
			AddNode(EdGraphNode);
		}
		else
		{
			CreateEdGraphNode(HeartGraphNode);
		}
	}
}

void UHeartEdGraph::OnNodeRemoved(UHeartGraphNode* HeartGraphNode)
{
	if (IsValid(HeartGraphNode))
	{
		if (auto&& EdGraphNode = FindEdGraphNodeForNode(HeartGraphNode))
		{
			EdGraphNode->DestroyNode();
		}
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
	const UEdGraphNode* EdNodeA = FindEdGraphNodeForNode(NodeA);
	const UEdGraphNode* EdNodeB = FindEdGraphNodeForNode(NodeB);
	const FHeartPinGuid PinAGuid = HeartGraphConnectionEvent.AffectedPins.Get(FSetElementId::FromInteger(0));
	const FHeartPinGuid PinBGuid = HeartGraphConnectionEvent.AffectedPins.Get(FSetElementId::FromInteger(1));

	if (EdNodeA && EdNodeB)
	{
		auto&& EdGraphPinA = EdNodeA->FindPin(NodeA->GetPinDescChecked(PinAGuid).Name);
		auto&& EdGraphPinB = EdNodeB->FindPin(NodeB->GetPinDescChecked(PinBGuid).Name);

		if (EdGraphPinA && EdGraphPinB)
		{
			if (NodeA->GetConnections(PinAGuid, true).Contains(NodeB->GetPinReference(PinBGuid)) &&
				NodeB->GetConnections(PinBGuid, true).Contains(NodeA->GetPinReference(PinAGuid)))
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