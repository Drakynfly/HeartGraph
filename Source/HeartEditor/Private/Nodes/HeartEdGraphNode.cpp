// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Nodes/HeartEdGraphNode.h"

//#include "Asset/HeartDebugger.h"
#include "HeartEditorCommands.h"
#include "Graph/HeartEdGraph.h"
#include "Graph/HeartEdGraphSchema.h"
#include "Graph/HeartEdGraphUtils.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

#include "UObject/ObjectSaveContext.h"

#include "Developer/ToolMenus/Public/ToolMenus.h"
#include "EdGraph/EdGraphSchema.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "HeartEditorModule.h"
#include "HeartRegistryEditorSubsystem.h"
#include "ScopedTransaction.h"
#include "Textures/SlateIcon.h"
#include "ToolMenuSection.h"
#include "HeartGraphStatics.h"
#include "Input/EdGraphPointerWrappers.h"
#include "Input/HeartInputLinkerBase.h"
#include "Input/HeartSlateInputLinker.h"
#include "ModelView/HeartGraphSchema.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartEdGraphNode)

#define LOCTEXT_NAMESPACE "HeartGraphNode"

UHeartEdGraphNode::UHeartEdGraphNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, HeartGraphNode(nullptr)
	, bBlueprintCompilationPending(false)
	, bNeedsFullReconstruction(false)
{
	OrphanedPinSaveMode = ESaveOrphanPinMode::SaveAll;
}

UHeartEdGraphNode::~UHeartEdGraphNode()
{
	UnsubscribeToExternalChanges();
}

UHeartGraphNode* UHeartEdGraphNode::GetHeartGraphNode() const
{
	if (ensure(IsValid(HeartGraphNode)))
	{
		return HeartGraphNode;
	}

	return nullptr;
}

void UHeartEdGraphNode::SetHeartGraphNode(UHeartGraphNode* InHeartGraphNode)
{
	HeartGraphNode = InHeartGraphNode;
}

void UHeartEdGraphNode::PostLoad()
{
	Super::PostLoad();

	if (!IsTemplate())
	{
		SubscribeToExternalChanges();

		ReconstructNode();
	}
}

void UHeartEdGraphNode::PostDuplicate(const EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);

	if (DuplicateMode != EDuplicateMode::PIE)
	{
		CreateNewGuid();

		if (IsValid(HeartGraphNode) && HeartGraphNode->GetGraph())
		{
			// @todo this check ensures that nodes are not duplicated at runtime, when Duplicating a HeartGraph:
			// Doing so propogates sub-object DuplicateObject calls to us, and results in HeartNodes being added to the
			// graph twice. This is all kinda ugly, since the code below is a hack to fix duplicating nodes in the first
			// place, and now this code is a secondary hack to fix the first, so really, the node duplication stuff below
			// needs to be moved elsewhere, then this can be removed too.
			if (!HeartGraphNode->GetGraph()->GetEdGraph())
			{
				return;
			}

			auto&& DuplicatedNode = DuplicateObject(HeartGraphNode, HeartGraphNode->GetOuter());
			DuplicatedNode->Guid = FHeartNodeGuid::New();
			DuplicatedNode->Location = FVector2D(NodePosX, NodePosY);


			// If the Graph Node's Object is owned within the graph, we should make a copy of it.
			if (HeartGraphNode->NodeObject->GetOuter() == HeartGraphNode ||
				HeartGraphNode->NodeObject->GetOuter() == HeartGraphNode->GetGraph())
			{
				DuplicatedNode->NodeObject = DuplicateObject(HeartGraphNode->NodeObject, HeartGraphNode->GetGraph());
			}
			// Otherwise, its an external asset, that we can both reference.
			else
			{
				DuplicatedNode->NodeObject = HeartGraphNode->NodeObject;
			}


			HeartGraphNode->GetGraph()->AddNode(DuplicatedNode);
			HeartGraphNode = DuplicatedNode;
		}
	}
}

void UHeartEdGraphNode::PostEditImport()
{
	Super::PostEditImport();

	PostCopyNode();
	SubscribeToExternalChanges();
}

void UHeartEdGraphNode::PreSave(const FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	// Sync runtime location with position in edgraph
	// @todo maybe move this to the edgraph's presave, where it can bulk edit validation stuff like this
	if (IsValid(HeartGraphNode))
	{
		FVector2D NewLocation;
		NewLocation.X = NodePosX;
		NewLocation.Y = NodePosY;
		HeartGraphNode->SetLocation(NewLocation);
		HeartGraphNode->GetGraph()->NotifyNodeLocationsChanged({HeartGraphNode}, false);
	}
}

void UHeartEdGraphNode::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();

	SubscribeToExternalChanges();
}

void UHeartEdGraphNode::PrepareForCopying()
{
	Super::PrepareForCopying();

	if (HeartGraphNode)
	{
		// Temporarily take ownership of the HeartGraphNode, so that it is not deleted when cutting
		HeartGraphNode->Rename(nullptr, this, REN_DontCreateRedirectors);
	}
}

void UHeartEdGraphNode::PostPasteNode()
{
	Super::PostPasteNode();

	if (IsValid(HeartGraphNode) && HeartGraphNode->GetGraph())
	{
		// If the Graph Node's Object is owned within the graph, we should make a copy of it.
		if (HeartGraphNode->NodeObject->GetOuter() == HeartGraphNode ||
			HeartGraphNode->NodeObject->GetOuter() == HeartGraphNode->GetGraph())
		{
			HeartGraphNode->NodeObject = DuplicateObject(HeartGraphNode->NodeObject, HeartGraphNode->GetGraph());
		}
		// Otherwise, its an external asset, that we can both reference.
	}
}

void UHeartEdGraphNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	UHeartGraph* HeartGraph = HeartGraphNode->GetGraph();

	// Get the matching HeartPin for the EdGraphPin that was changed
	const FHeartPinGuid& HeartPin = HeartGraphNode->GetPinByName(Pin->PinName);

	const FHeartGraphPinReference SelfReference{HeartGraphNode->GetGuid(), HeartPin};

	if (!ensure(HeartPin.IsValid()))
	{
		UE_LOG(LogHeartEditor, Error, TEXT("Changed UEdGraphPin does not have a runtime equivilant!"))
		return;
	}

	TArray<FHeartGraphPinDesc> LinkedPins;

	Heart::Connections::FEdit ConnectionEditor = HeartGraph->EditConnections();

	// Resolve all linked pins
	if (TOptional<FHeartGraphPinConnections> PinConnections = HeartGraphNode->GetConnections(HeartPin);
		PinConnections.IsSet())
	{
		for (auto&& LinkedRef : PinConnections.GetValue().Links)
		{
			const UHeartGraphNode* LinkedNode = HeartGraph->GetNode(LinkedRef.NodeGuid);
			if (!IsValid(LinkedNode))
			{
				UE_LOG(LogHeartEditor, Warning, TEXT("HeartGraphNode '%s' has an invalid Linked Node. It should be fixed up!"),
					*HeartGraphNode.GetName())
				continue;
			}

			auto&& LinkedPin = LinkedNode->GetPinDesc(LinkedRef.PinGuid);
			if (!LinkedPin.IsSet())
			{
				UE_LOG(LogHeartEditor, Warning, TEXT("HeartGraphNode '%s' has an invalid Linked Pin to node '%s'. It should be fixed up!"),
					*HeartGraphNode.GetName(), *LinkedNode->GetName())
				continue;
			}

			LinkedPins.Add(LinkedPin.GetValue());

			if (!Pin->LinkedTo.ContainsByPredicate(
					[Desc = LinkedPin.GetValue()](const UEdGraphPin* EdGraphPin)
					{
						return Desc.Name == EdGraphPin->PinName;
					}))
			{
				// If we failed to find a connection in the EdGraph, then we need to disconnect the runtime pins
				ConnectionEditor.Disconnect(LinkedRef, SelfReference);
			}
		}
	}

	// Ensure EdGraph Links are synced with HeartGraph links
	for (auto&& EdGraphPin : Pin->LinkedTo)
	{
		bool FoundConnection = false;

		for (auto&& LinkedPin : LinkedPins)
		{
			// Find the one that matches the EdGraphPin
			if (LinkedPin.Name == EdGraphPin->PinName)
			{
				FoundConnection = true;
				break;
			}
		}

		// If we failed to find a connection, then we need to connect the runtime pins
		if (!FoundConnection)
		{
			const UHeartGraphNode* HeartNodeConnectedInEditor = Cast<UHeartEdGraphNode>(EdGraphPin->GetOwningNode())->GetHeartGraphNode();
			const FHeartPinGuid& ConnectedHeartPin = HeartNodeConnectedInEditor->GetPinByName(EdGraphPin->PinName);

			if (!ensure(ConnectedHeartPin.IsValid()))
			{
				UE_LOG(LogHeartEditor, Error, TEXT("Changed HeartEdGraphNode does not have a runtime equivilant!"))
				break;
			}

			ConnectionEditor.Connect(SelfReference, {HeartNodeConnectedInEditor->GetGuid(), ConnectedHeartPin});
		}
	}
}

void UHeartEdGraphNode::OnHeartGraphNodePropertyChanged(UObject* Obj, FPropertyChangedEvent& PropertyChangedEvent)
{
	if (Obj != HeartGraphNode)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty &&
	   (PropertyChangedEvent.MemberProperty->HasMetaData(Heart::Graph::Metadata_TriggersReconstruct) ||
		HeartGraphNode->GetPropertiesTriggeringNodeReconstruction().Contains(PropertyChangedEvent.GetPropertyName())))
	{
		OnNodeRequestReconstruction();
	}
}

void UHeartEdGraphNode::PostCopyNode()
{
	// Make sure this HeartGraphNode is owned by the HeartGraph it's being pasted into
	if (HeartGraphNode)
	{
		auto&& HeartGraph = CastChecked<UHeartEdGraph>(GetGraph())->GetHeartGraph();

		if (HeartGraphNode->GetOuter() != HeartGraph)
		{
			// Ensures HeartGraphNode is owned by the HeartGraph
			HeartGraphNode->Rename(nullptr, HeartGraph, REN_DontCreateRedirectors);
		}
	}
}

void UHeartEdGraphNode::SubscribeToExternalChanges()
{
	if (ensure(IsValid(HeartGraphNode)))
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.AddUObject(this, &ThisClass::OnHeartGraphNodePropertyChanged);

		// blueprint nodes
		if (HeartGraphNode->GetClass()->ClassGeneratedBy && GEditor)
		{
			GEditor->OnBlueprintPreCompile().AddUObject(this, &UHeartEdGraphNode::OnBlueprintPreCompile);
			GEditor->OnBlueprintCompiled().AddUObject(this, &UHeartEdGraphNode::OnBlueprintCompiled);
		}
	}
}

void UHeartEdGraphNode::UnsubscribeToExternalChanges()
{
	if (UObjectInitialized())
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
	}

	if (GEditor)
	{
		GEditor->OnBlueprintPreCompile().RemoveAll(this);
		GEditor->OnBlueprintCompiled().RemoveAll(this);
	}
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void UHeartEdGraphNode::OnBlueprintPreCompile(UBlueprint* Blueprint)
{
	if (Blueprint && Blueprint == HeartGraphNode->GetClass()->ClassGeneratedBy)
	{
		bBlueprintCompilationPending = true;

		UObject* NodeObject = HeartGraphNode->GetNodeObject();
		if (NodeObject->GetOuter() == HeartGraphNode)
		{
			// While we are compiling, our NodeObject needs to be moved temporarily to the EdGraph for ownership.
			// If we don't then it gets destroyed for some reason! (relevant for 5.2, retest on future versions)
			NodeObject->Rename(nullptr, GetGraph());
		}
	}
}

void UHeartEdGraphNode::OnBlueprintCompiled()
{
	if (bBlueprintCompilationPending)
	{
		// Restore the node from the above hack, if it was applied.
		UObject* NodeObject = HeartGraphNode->GetNodeObject();
		if (ensure(IsValid(NodeObject)))
		{
			if (NodeObject->GetOuter() == GetGraph())
			{
				NodeObject->Rename(nullptr, HeartGraphNode);
			}
		}

		OnNodeRequestReconstruction();
	}

	bBlueprintCompilationPending = false;
}

void UHeartEdGraphNode::OnNodeRequestReconstruction()
{
	bNeedsFullReconstruction = true;

	ReconstructNode();
	GetGraph()->NotifyGraphChanged();
}

bool UHeartEdGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema->IsA(UHeartEdGraphSchema::StaticClass());
}

void UHeartEdGraphNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (FromPin != nullptr)
	{
		const UHeartEdGraphSchema* Schema = CastChecked<UHeartEdGraphSchema>(GetSchema());

		TSet<UEdGraphNode*> NodeList;

		// auto-connect from dragged pin to first compatible pin on the new node
		for (UEdGraphPin* Pin : Pins)
		{
			check(Pin);
			FPinConnectionResponse Response = Schema->CanCreateConnection(FromPin, Pin);
			if (CONNECT_RESPONSE_MAKE == Response.Response)
			{
				if (Schema->TryCreateConnection(FromPin, Pin))
				{
					NodeList.Add(FromPin->GetOwningNode());
					NodeList.Add(this);
				}
				break;
			}
			else if (CONNECT_RESPONSE_BREAK_OTHERS_A == Response.Response)
			{
				InsertNewNode(FromPin, Pin, NodeList);
				break;
			}
		}

		// Send all nodes that received a new pin connection a notification
		for (auto It = NodeList.CreateConstIterator(); It; ++It)
		{
			UEdGraphNode* Node = (*It);
			Node->NodeConnectionListChanged();
		}
	}
}

void UHeartEdGraphNode::InsertNewNode(UEdGraphPin* FromPin, UEdGraphPin* NewLinkPin, TSet<UEdGraphNode*>& OutNodeList)
{
	const UHeartEdGraphSchema* Schema = CastChecked<UHeartEdGraphSchema>(GetSchema());

	// The pin we are creating from already has a connection that needs to be broken. We want to "insert" the new node in between, so that the output of the new node is hooked up too
	UEdGraphPin* OldLinkedPin = FromPin->LinkedTo[0];
	check(OldLinkedPin);

	FromPin->BreakAllPinLinks();

	// Hook up the old linked pin to the first valid output pin on the new node
	for (int32 PinIndex = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		UEdGraphPin* OutputExecPin = Pins[PinIndex];
		check(OutputExecPin);
		if (CONNECT_RESPONSE_MAKE == Schema->CanCreateConnection(OldLinkedPin, OutputExecPin).Response)
		{
			if (Schema->TryCreateConnection(OldLinkedPin, OutputExecPin))
			{
				OutNodeList.Add(OldLinkedPin->GetOwningNode());
				OutNodeList.Add(this);
			}
			break;
		}
	}

	if (Schema->TryCreateConnection(FromPin, NewLinkPin))
	{
		OutNodeList.Add(FromPin->GetOwningNode());
		OutNodeList.Add(this);
	}
}

void UHeartEdGraphNode::ReconstructNode()
{
	// Store old pins
	TArray<UEdGraphPin*> OldPins(Pins);

	// Reset pin arrays
	Pins.Reset();
	InputPins.Reset();
	OutputPins.Reset();

	// Recreate pins
	AllocateDefaultPins();
	RewireOldPinsToNewPins(OldPins);

	// Destroy old pins
	for (UEdGraphPin* OldPin : OldPins)
	{
		OldPin->Modify();
		OldPin->BreakAllPinLinks();
		DestroyPin(OldPin);
	}

	bNeedsFullReconstruction = false;
}

void UHeartEdGraphNode::AllocateDefaultPins()
{
	check(Pins.Num() == 0);

	if (!IsValid(HeartGraphNode))
	{
		return;
	}

	static constexpr bool Sorted = true;
	TArray<FHeartPinGuid> ExistingInputPins = HeartGraphNode->GetInputPins(Sorted);
	TArray<FHeartPinGuid> ExistingOutputPins = HeartGraphNode->GetOutputPins(Sorted);

	for (const FHeartPinGuid& InputPin : ExistingInputPins)
	{
		if (!ensure(InputPin.IsValid()))
		{
			continue;
		}

		CreateInputPin(HeartGraphNode->GetPinDescChecked(InputPin));
	}

	for (const FHeartPinGuid& OutputPin : ExistingOutputPins)
	{
		if (!ensure(OutputPin.IsValid()))
		{
			continue;
		}

		CreateOutputPin(HeartGraphNode->GetPinDescChecked(OutputPin));
	}
}

void UHeartEdGraphNode::RewireOldPinsToNewPins(TArray<UEdGraphPin*>& InOldPins)
{
	TArray<UEdGraphPin*> OrphanedOldPins;
	TArray<bool> NewPinMatched; // Tracks whether a NewPin has already been matched to an OldPin
	TMap<UEdGraphPin*, UEdGraphPin*> MatchedPins; // Old to New

	const int32 NumNewPins = Pins.Num();
	NewPinMatched.AddDefaulted(NumNewPins);

	// Rewire any connection to pins that are matched by name (O(N^2) right now)
	// NOTE: we iterate backwards through the list because ReconstructSinglePin()
	//       destroys pins as we go along (clearing out parent pointers, etc.);
	//       we need the parent pin chain intact for DoPinsMatchForReconstruction();
	//       we want to destroy old pins from the split children (leaves) up, so
	//       we do this since split child pins are ordered later in the list
	//       (after their parents)
	for (int32 OldPinIndex = InOldPins.Num() - 1; OldPinIndex >= 0; --OldPinIndex)
	{
		UEdGraphPin* OldPin = InOldPins[OldPinIndex];

		// common case is for InOldPins and Pins to match, so we start searching from the current index:
		bool bMatched = false;
		int32 NewPinIndex = (NumNewPins ? OldPinIndex % NumNewPins : 0);
		for (int32 NewPinCount = NumNewPins - 1; NewPinCount >= 0; --NewPinCount)
		{
			// if Pins grows then we may skip entries and fail to find a match or NewPinMatched will not be accurate
			check(NumNewPins == Pins.Num());
			if (!NewPinMatched[NewPinIndex])
			{
				UEdGraphPin* NewPin = Pins[NewPinIndex];

				if (NewPin->PinName == OldPin->PinName)
				{
					ReconstructSinglePin(NewPin, OldPin);

					MatchedPins.Add(OldPin, NewPin);
					bMatched = true;
					NewPinMatched[NewPinIndex] = true;
					break;
				}
			}
			NewPinIndex = (NewPinIndex + 1) % Pins.Num();
		}

		// Orphaned pins are those that existed in the OldPins array but do not in the NewPins.
		// We will save these pins and add them to the NewPins array if they are linked to other pins or have non-default value unless:
		// * The node has been flagged to not save orphaned pins
		// * The pin has been flagged not be saved if orphaned
		// * The pin is hidden
		if (UEdGraphPin::AreOrphanPinsEnabled() && !bDisableOrphanPinSaving && OrphanedPinSaveMode == ESaveOrphanPinMode::SaveAll
			&& !bMatched && !OldPin->bHidden && OldPin->ShouldSavePinIfOrphaned() && OldPin->LinkedTo.Num() > 0)
		{
			OldPin->bOrphanedPin = true;
			OldPin->bNotConnectable = true;
			OrphanedOldPins.Add(OldPin);
			InOldPins.RemoveAt(OldPinIndex, 1, false);
		}
	}

	// The orphaned pins get placed after the rest of the new pins
	for (int32 OrphanedIndex = OrphanedOldPins.Num() - 1; OrphanedIndex >= 0; --OrphanedIndex)
	{
		UEdGraphPin* OrphanedPin = OrphanedOldPins[OrphanedIndex];
		if (OrphanedPin->ParentPin == nullptr)
		{
			Pins.Add(OrphanedPin);
		}
	}
}

void UHeartEdGraphNode::ReconstructSinglePin(UEdGraphPin* NewPin, UEdGraphPin* OldPin)
{
	check(NewPin && OldPin);

	// Copy over modified persistent data
	NewPin->MovePersistentDataFromOldPin(*OldPin);

	// Update the in breakpoints as the old pin will be going the way of the dodo
	for (TPair<FEdGraphPinReference, FHeartBreakpoint>& PinBreakpoint : PinBreakpoints)
	{
		if (PinBreakpoint.Key.Get() == OldPin)
		{
			PinBreakpoint.Key = NewPin;
			break;
		}
	}
}

void UHeartEdGraphNode::GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	const FGenericCommands& GenericCommands = FGenericCommands::Get();
	const FGraphEditorCommandsImpl& GraphCommands = FGraphEditorCommands::Get();
	const FHeartGraphCommands& HeartGraphCommands = FHeartGraphCommands::Get();

	if (Context->Pin)
	{
		{
			FToolMenuSection& Section = Menu->AddSection("HeartGraphPinActions", LOCTEXT("PinActionsMenuHeader", "Pin Actions"));
			if (Context->Pin->LinkedTo.Num() > 0)
			{
				Section.AddMenuEntry(GraphCommands.BreakPinLinks);
			}

			if (Context->Pin->Direction == EGPD_Input && CanUserRemoveInput(Context->Pin))
			{
				Section.AddMenuEntry(HeartGraphCommands.RemovePin);
			}
			else if (Context->Pin->Direction == EGPD_Output && CanUserRemoveOutput(Context->Pin))
			{
				Section.AddMenuEntry(HeartGraphCommands.RemovePin);
			}
		}

		{
			FToolMenuSection& Section = Menu->AddSection("HeartGraphPinBreakpoints", LOCTEXT("PinBreakpointsMenuHeader", "Pin Breakpoints"));
			Section.AddMenuEntry(HeartGraphCommands.AddPinBreakpoint);
			Section.AddMenuEntry(HeartGraphCommands.RemovePinBreakpoint);
			Section.AddMenuEntry(HeartGraphCommands.EnablePinBreakpoint);
			Section.AddMenuEntry(HeartGraphCommands.DisablePinBreakpoint);
			Section.AddMenuEntry(HeartGraphCommands.TogglePinBreakpoint);
		}

		{
			/** Linker Actions **/
			FToolMenuSection& Section = Menu->AddSection("HeartGraphPinLinkerActions", LOCTEXT("PinLinkerActions", "Linker Actions"));

			auto&& HeartEdGraph = Cast<UHeartEdGraph>(GetGraph());
			auto&& Linker = HeartEdGraph->GetEditorLinker();
			if (IsValid(Linker))
			{
				TArray<FHeartManualInputQueryResult> QueryResults = Linker->QueryManualTriggers(UHeartEdGraphPin::Wrap(Context->Pin));

				for (auto&& QueryResult : QueryResults)
				{
					// @todo
				}
			}
		}
	}
	else if (Context->Node)
	{
		{
			FToolMenuSection& Section = Menu->AddSection("HeartGraphNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
			Section.AddMenuEntry(GenericCommands.Delete);
			Section.AddMenuEntry(GenericCommands.Cut);
			Section.AddMenuEntry(GenericCommands.Copy);
			Section.AddMenuEntry(GenericCommands.Duplicate);

			Section.AddMenuEntry(GraphCommands.BreakNodeLinks);

			if (CanUserAddInput())
			{
				Section.AddMenuEntry(HeartGraphCommands.AddInput);
			}
			if (CanUserAddOutput())
			{
				Section.AddMenuEntry(HeartGraphCommands.AddOutput);
			}
		}

		{
			FToolMenuSection& Section = Menu->AddSection("HeartGraphNodeBreakpoints", LOCTEXT("NodeBreakpointsMenuHeader", "Node Breakpoints"));
			Section.AddMenuEntry(GraphCommands.AddBreakpoint);
			Section.AddMenuEntry(GraphCommands.RemoveBreakpoint);
			Section.AddMenuEntry(GraphCommands.EnableBreakpoint);
			Section.AddMenuEntry(GraphCommands.DisableBreakpoint);
			Section.AddMenuEntry(GraphCommands.ToggleBreakpoint);
		}

		{
			FToolMenuSection& Section = Menu->AddSection("HeartGraphNodeJumps", LOCTEXT("NodeJumpsMenuHeader", "Jumps"));
			if (CanJumpToDefinition())
			{
				Section.AddMenuEntry(HeartGraphCommands.JumpToGraphNodeDefinition);
				Section.AddMenuEntry(HeartGraphCommands.JumpToNodeDefinition);
			}
		}

		{
			/** Linker Actions **/
			FToolMenuSection& Section = Menu->AddSection("HeartGraphNodeLinkerActions", LOCTEXT("NodeLinkerActions", "Linker Actions"));

			auto&& HeartEdGraph = Cast<UHeartEdGraph>(GetGraph());
			auto&& Linker = HeartEdGraph->GetEditorLinker();
			if (IsValid(Linker))
			{
				TArray<FHeartManualInputQueryResult> QueryResults = Linker->QueryManualTriggers(Context->Node);

				for (auto&& QueryResult : QueryResults)
				{
					// @todo
				}
			}
		}
	}
}

bool UHeartEdGraphNode::CanUserDeleteNode() const
{
	return HeartGraphNode ? HeartGraphNode->CanDelete() : Super::CanUserDeleteNode();
}

bool UHeartEdGraphNode::CanDuplicateNode() const
{
	return HeartGraphNode ? HeartGraphNode->CanDuplicate() : Super::CanDuplicateNode();
}

TSharedPtr<SGraphNode> UHeartEdGraphNode::CreateVisualWidget()
{
	if (HeartGraphNode)
	{
		FName SlateStyle = HeartGraphNode->GetEditorSlateStyle();

		if (SlateStyle == Heart::GraphUtils::DefaultStyle)
		{
			SlateStyle = HeartGraphNode->GetGraph()->GetSchema()->GetDefaultEditorStyle();
		}

		auto&& EditorRegister = GEditor->GetEditorSubsystem<UHeartRegistryEditorSubsystem>();
		return EditorRegister->MakeSlateWidget(SlateStyle, this);
	}

	return Super::CreateVisualWidget();
}

FText UHeartEdGraphNode::GetNodeTitle(const ENodeTitleType::Type TitleType) const
{
	// When called on CDO, fallback to super's default behavior.
	if (IsTemplate())
	{
		return Super::GetNodeTitle(TitleType);
	}

	// Non-templates should always have a valid GraphNode & NodeObject

	if (!ensure(IsValid(HeartGraphNode)))
	{
		// Display error with *our* name, in case we are a custom EdGraphNode that may help debug the situation.
		return FText::Format(
			LOCTEXT("GetNodeTitle_InvalidGraphNode", "[Invalid GraphNode ({0})]"), FText::FromString(GetName()));
	}

	if (!IsValid(HeartGraphNode->GetNodeObject()))
	{
		// Display error with the GraphNode name, that may help debug the situation.
		return FText::Format(
			LOCTEXT("GetNodeTitle_InvalidNodeObject", "[Invalid NodeObject ({0})]"), FText::FromString(HeartGraphNode->GetName()));
	}

	{
		FEditorScriptExecutionGuard EditorScriptExecutionGuard;
		{
			switch (TitleType)
			{
				// @todo support for EditableTitles when? :)
			case ENodeTitleType::EditableTitle:
				{
					return FText::GetEmpty();
				}

				// @todo is full FullTitle only used for in-graph instances? i *think* so
			case ENodeTitleType::FullTitle:
				return HeartGraphNode->GetInstanceTitle();

			case ENodeTitleType::MenuTitle:

				// ListView is used by a few slate classes to get a short name from instanced nodes.
			case ENodeTitleType::ListView:
			default: ;
				EHeartPreviewNodeNameContext Context = EHeartPreviewNodeNameContext::Default;

				if (TitleType == ENodeTitleType::MenuTitle)
				{
					Context = EHeartPreviewNodeNameContext::Palette;
				}

				// If the NodeObject is outer'd this was created from a class source
				if (HeartGraphNode->NodeObject->GetOuter() == HeartGraphNode)
				{
					return HeartGraphNode->GetPreviewNodeTitle(FHeartNodeSource(HeartGraphNode->NodeObject->GetClass()), Context);
				}

				// Otherwise, the NodeObject itself is the source
				return HeartGraphNode->GetPreviewNodeTitle(FHeartNodeSource(HeartGraphNode->NodeObject), Context);
			}
		}
	}
}

FLinearColor UHeartEdGraphNode::GetNodeTitleColor() const
{
	if (HeartGraphNode)
	{
		return HeartGraphNode->GetNodeTitleColor(HeartGraphNode->GetNodeObject());
	}

	return Super::GetNodeTitleColor();
}

FSlateIcon UHeartEdGraphNode::GetIconAndTint(FLinearColor& OutColor) const
{
	return FSlateIcon();
}

FText UHeartEdGraphNode::GetTooltipText() const
{
	FText Tooltip;
	if (HeartGraphNode)
	{
		Tooltip = HeartGraphNode->GetClass()->GetToolTipText();
	}
	if (Tooltip.IsEmpty())
	{
		Tooltip = GetNodeTitle(ENodeTitleType::ListView);
	}
	return Tooltip;
}

FEdGraphPinType UHeartEdGraphNode::GetEdGraphPinTypeFromPinDesc(const FHeartGraphPinDesc& PinDesc) const
{
	static FEdGraphPinType DefaultEdGraphPinType = FEdGraphPinType("exec", NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType());
	return DefaultEdGraphPinType;
}

bool UHeartEdGraphNode::CanJumpToDefinition() const
{
	return HeartGraphNode != nullptr;
}

void UHeartEdGraphNode::JumpToDefinition() const
{
	if (ensure(IsValid(HeartGraphNode)))
	{
		Heart::GraphUtils::JumpToClassDefinition(HeartGraphNode->GetClass());
	}
}

void UHeartEdGraphNode::GetPopupMessages(TArray<TPair<FString, FLinearColor>>& Messages) const
{
	TArray<FHeartGraphNodeMessage> RuntimeMessages;
	HeartGraphNode->GetNodeMessages(RuntimeMessages);

	for (const FHeartGraphNodeMessage& Tuple : RuntimeMessages)
	{
		Messages.Add({Tuple.Message.ToString(), Tuple.Color});
	}
}

void UHeartEdGraphNode::JumpToNodeDefinition() const
{
	if (ensure(IsValid(HeartGraphNode)))
	{
		if (ensure(IsValid(HeartGraphNode->GetNodeObject())))
		{
			Heart::GraphUtils::JumpToClassDefinition(HeartGraphNode->GetNodeObject()->GetClass());
		}
	}
}

void UHeartEdGraphNode::CreateInputPin(const FHeartGraphPinDesc& PinDesc)
{
	if (!ensure(PinDesc.IsValid()))
	{
		return;
	}

	UEdGraphPin* NewPin = CreatePin(EGPD_Input, GetEdGraphPinTypeFromPinDesc(PinDesc), PinDesc.Name);
	check(NewPin);

	if (!PinDesc.FriendlyName.IsEmpty())
	{
		NewPin->bAllowFriendlyName = true;
		NewPin->PinFriendlyName = PinDesc.FriendlyName;
	}

	if (!PinDesc.Tooltip.IsEmpty())
	{
		NewPin->PinToolTip.Append(PinDesc.Tooltip.ToString());
	}
	if (!PinDesc.EditorTooltip.IsEmpty())
	{
		NewPin->PinToolTip += LINE_TERMINATOR LINE_TERMINATOR;
		NewPin->PinToolTip.Append(PinDesc.EditorTooltip.ToString());
	}

	InputPins.Emplace(NewPin);
}

void UHeartEdGraphNode::CreateOutputPin(const FHeartGraphPinDesc& PinDesc)
{
	if (!ensure(PinDesc.IsValid()))
	{
		return;
	}

	UEdGraphPin* NewPin = CreatePin(EGPD_Output, GetEdGraphPinTypeFromPinDesc(PinDesc), PinDesc.Name);
	check(NewPin);

	if (!PinDesc.FriendlyName.IsEmpty())
	{
		NewPin->bAllowFriendlyName = true;
		NewPin->PinFriendlyName = PinDesc.FriendlyName;
	}

	if (!PinDesc.Tooltip.IsEmpty())
	{
		NewPin->PinToolTip.Append(PinDesc.Tooltip.ToString());
	}
	if (!PinDesc.EditorTooltip.IsEmpty())
	{
		NewPin->PinToolTip += LINE_TERMINATOR LINE_TERMINATOR;
		NewPin->PinToolTip.Append(PinDesc.EditorTooltip.ToString());
	}

	OutputPins.Emplace(NewPin);
}

void UHeartEdGraphNode::RemoveOrphanedPin(UEdGraphPin* Pin)
{
	const FScopedTransaction Transaction(LOCTEXT("RemoveOrphanedPin", "Remove Orphaned Pin"));
	Modify();

	PinBreakpoints.Remove(Pin);

	Pin->MarkAsGarbage();
	Pins.Remove(Pin);

	ReconstructNode();
	GetGraph()->NotifyGraphChanged();
}

bool UHeartEdGraphNode::CanUserAddInput() const
{
	return HeartGraphNode && HeartGraphNode->CanUserAddInput() && InputPins.Num() < 256;
}

bool UHeartEdGraphNode::CanUserAddOutput() const
{
	return HeartGraphNode && HeartGraphNode->CanUserAddOutput() && OutputPins.Num() < 256;
}

bool UHeartEdGraphNode::CanUserRemoveInput(const UEdGraphPin* Pin) const
{
	if (!HeartGraphNode)
	{
		return false;
	}

	auto&& DefaultPins = HeartGraphNode->GetClass()->GetDefaultObject<UHeartGraphNode>()->GetDefaultPins();

	// Don't allow user to delete a default pin.
	for (auto&& DefaultPin : DefaultPins)
	{
		if (DefaultPin.Name == Pin->PinName)
		{
			return false;
		}
	}

	return true;
}

bool UHeartEdGraphNode::CanUserRemoveOutput(const UEdGraphPin* Pin) const
{
	if (!HeartGraphNode)
	{
		return false;
	}

	auto&& DefaultPins = HeartGraphNode->GetClass()->GetDefaultObject<UHeartGraphNode>()->GetDefaultPins();

	// Don't allow user to delete a default pin.
	for (auto&& DefaultPin : DefaultPins)
	{
		if (DefaultPin.Name == Pin->PinName)
		{
			return false;
		}
	}

	return true;
}

void UHeartEdGraphNode::AddUserInput()
{
	AddInstancePin(EGPD_Input);
}

void UHeartEdGraphNode::AddUserOutput()
{
	AddInstancePin(EGPD_Output);
}

void UHeartEdGraphNode::AddInstancePin(const EEdGraphPinDirection Direction)
{
	const FScopedTransaction Transaction(LOCTEXT("AddInstancePin", "Add Instance Pin"));
	Modify();

	if (Direction == EGPD_Input)
	{
		const FHeartPinGuid& NewInstancePin = HeartGraphNode->AddInstancePin(EHeartPinDirection::Input);
		auto&& Desc = HeartGraphNode->GetPinDescChecked(NewInstancePin);
		CreateInputPin(Desc);
		HeartGraphNode->AddPin(Desc);
	}
	else
	{
		const FHeartPinGuid& NewInstancePin = HeartGraphNode->AddInstancePin(EHeartPinDirection::Output);
		auto&& Desc = HeartGraphNode->GetPinDescChecked(NewInstancePin);
		CreateOutputPin(Desc);
		HeartGraphNode->AddPin(Desc);
	}

	GetGraph()->NotifyGraphChanged();
}

void UHeartEdGraphNode::RemoveInstancePin(UEdGraphPin* Pin)
{
	const FScopedTransaction Transaction(LOCTEXT("RemoveInstancePin", "Remove Instance Pin"));
	Modify();

	PinBreakpoints.Remove(Pin);

	if (Pin->Direction == EGPD_Input)
	{
		if (InputPins.Contains(Pin))
		{
			InputPins.Remove(Pin);

			Pin->MarkAsGarbage();
			Pins.Remove(Pin);
		}
	}
	else
	{
		if (OutputPins.Contains(Pin))
		{
			OutputPins.Remove(Pin);

			Pin->MarkAsGarbage();
			Pins.Remove(Pin);
		}
	}

	HeartGraphNode->RemovePinsByPredicate(Pin->Direction == EGPD_Output ? EHeartPinDirection::Output : EHeartPinDirection::Input,
		[Pin](FHeartPinGuid, const FHeartGraphPinDesc& Desc)
			{
				return Pin->PinName == Desc.Name;
			});

	ReconstructNode();
	GetGraph()->NotifyGraphChanged();
}

void UHeartEdGraphNode::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);
}

void UHeartEdGraphNode::OnInputTriggered(const int32 Index)
{
	if (InputPins.IsValidIndex(Index) && PinBreakpoints.Contains(InputPins[Index]))
	{
		PinBreakpoints[InputPins[Index]].bBreakpointHit = true;
		TryPausingSession(true);
	}

	TryPausingSession(false);
}

void UHeartEdGraphNode::OnOutputTriggered(const int32 Index)
{
	if (OutputPins.IsValidIndex(Index) && PinBreakpoints.Contains(OutputPins[Index]))
	{
		PinBreakpoints[OutputPins[Index]].bBreakpointHit = true;
		TryPausingSession(true);
	}

	TryPausingSession(false);
}

void UHeartEdGraphNode::TryPausingSession(bool bPauseSession)
{
	// Node breakpoints waits on any pin triggered
	if (NodeBreakpoint.IsBreakpointEnabled())
	{
		NodeBreakpoint.bBreakpointHit = true;
		bPauseSession = true;
	}

	if (bPauseSession)
	{
		FEditorDelegates::ResumePIE.AddUObject(this, &UHeartEdGraphNode::OnResumePIE);
		FEditorDelegates::EndPIE.AddUObject(this, &UHeartEdGraphNode::OnEndPIE);

		//FHeartDebugger::PausePlaySession();
	}
}

void UHeartEdGraphNode::OnResumePIE(const bool bIsSimulating)
{
	ResetBreakpoints();
}

void UHeartEdGraphNode::OnEndPIE(const bool bIsSimulating)
{
	ResetBreakpoints();
}

void UHeartEdGraphNode::ResetBreakpoints()
{
	FEditorDelegates::ResumePIE.RemoveAll(this);
	FEditorDelegates::EndPIE.RemoveAll(this);

	NodeBreakpoint.bBreakpointHit = false;
	for (TPair<FEdGraphPinReference, FHeartBreakpoint>& PinBreakpoint : PinBreakpoints)
	{
		PinBreakpoint.Value.bBreakpointHit = false;
	}
}

#undef LOCTEXT_NAMESPACE