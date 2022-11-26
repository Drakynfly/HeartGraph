// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Nodes/HeartEdGraphNode.h"

//#include "Asset/FlowDebugger.h"
#include "HeartEditorCommands.h"
#include "Graph/HeartEdGraph.h"
#include "Graph/HeartEdGraphSchema.h"
#include "Graph/Widgets/SHeartGraphNode.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"
#include "Model/HeartGraphPin.h"

#include "Developer/ToolMenus/Public/ToolMenus.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "ScopedTransaction.h"
#include "SourceCodeNavigation.h"
#include "Textures/SlateIcon.h"
#include "ToolMenuSection.h"
#include "UnrealEd.h"

#define LOCTEXT_NAMESPACE "HeartGraphNode"

//////////////////////////////////////////////////////////////////////////
// Heart Breakpoint

void FHeartBreakpoint::AddBreakpoint()
{
	if (!bHasBreakpoint)
	{
		bHasBreakpoint = true;
		bBreakpointEnabled = true;
	}
}

void FHeartBreakpoint::RemoveBreakpoint()
{
	if (bHasBreakpoint)
	{
		bHasBreakpoint = false;
		bBreakpointEnabled = false;
	}
}

bool FHeartBreakpoint::HasBreakpoint() const
{
	return bHasBreakpoint;
}

void FHeartBreakpoint::EnableBreakpoint()
{
	if (bHasBreakpoint && !bBreakpointEnabled)
	{
		bBreakpointEnabled = true;
	}
}

bool FHeartBreakpoint::CanEnableBreakpoint() const
{
	return bHasBreakpoint && !bBreakpointEnabled;
}

void FHeartBreakpoint::DisableBreakpoint()
{
	if (bHasBreakpoint && bBreakpointEnabled)
	{
		bBreakpointEnabled = false;
	}
}

bool FHeartBreakpoint::IsBreakpointEnabled() const
{
	return bHasBreakpoint && bBreakpointEnabled;
}

void FHeartBreakpoint::ToggleBreakpoint()
{
	if (bHasBreakpoint)
	{
		bHasBreakpoint = false;
		bBreakpointEnabled = false;
	}
	else
	{
		bHasBreakpoint = true;
		bBreakpointEnabled = true;
	}
}

//////////////////////////////////////////////////////////////////////////
// Heart Graph Node

UHeartEdGraphNode::UHeartEdGraphNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, HeartGraphNode(nullptr)
	, bBlueprintCompilationPending(false)
	, bNeedsFullReconstruction(false)
{
	OrphanedPinSaveMode = ESaveOrphanPinMode::SaveAll;
}

void UHeartEdGraphNode::SetHeartGraphNode(UHeartGraphNode* InHeartGraphNode)
{
	HeartGraphNode = InHeartGraphNode;
}

UHeartGraphNode* UHeartEdGraphNode::GetHeartGraphNode() const
{
	if (HeartGraphNode)
	{
		if (auto&& InspectedInstance = HeartGraphNode->GetGraph())
		{
			return InspectedInstance->GetNode(HeartGraphNode->GetGuid());
		}

		return HeartGraphNode;
	}

	return nullptr;
}

void UHeartEdGraphNode::PostLoad()
{
	Super::PostLoad();

	if (HeartGraphNode)
	{
		HeartGraphNode->SetEdGraphNode(this); // fix already created nodes
		SubscribeToExternalChanges();
	}

	ReconstructNode();
}

void UHeartEdGraphNode::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);

	if (!bDuplicateForPIE)
	{
		CreateNewGuid();

		if (HeartGraphNode && HeartGraphNode->GetGraph())
		{
			HeartGraphNode->GetGraph()->AddNode(HeartGraphNode);
		}
	}
}

void UHeartEdGraphNode::PostEditImport()
{
	Super::PostEditImport();

	PostCopyNode();
	SubscribeToExternalChanges();
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

		HeartGraphNode->SetEdGraphNode(this);
	}
}

void UHeartEdGraphNode::SubscribeToExternalChanges()
{
	if (HeartGraphNode)
	{
		HeartGraphNode->OnReconstructionRequested.AddDynamic(this, &ThisClass::OnExternalChange);

		// blueprint nodes
		if (HeartGraphNode->GetClass()->ClassGeneratedBy && GEditor)
		{
			GEditor->OnBlueprintPreCompile().AddUObject(this, &UHeartEdGraphNode::OnBlueprintPreCompile);
			GEditor->OnBlueprintCompiled().AddUObject(this, &UHeartEdGraphNode::OnBlueprintCompiled);
		}
	}
}

void UHeartEdGraphNode::OnBlueprintPreCompile(UBlueprint* Blueprint)
{
	if (Blueprint && Blueprint == HeartGraphNode->GetClass()->ClassGeneratedBy)
	{
		bBlueprintCompilationPending = true;
	}
}

void UHeartEdGraphNode::OnBlueprintCompiled()
{
	if (bBlueprintCompilationPending)
	{
		OnExternalChange(HeartGraphNode);
	}

	bBlueprintCompilationPending = false;
}

void UHeartEdGraphNode::OnExternalChange(UHeartGraphNode* Node)
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
	if (SupportsDynamicPins() && (/**HeartGraphNode->CanRefreshDynamicPinsOnLoad() ||*/ bNeedsFullReconstruction))
	{
		RefreshDynamicPins(false);
	}
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

	if (HeartGraphNode)
	{
		for (auto&& InputPin : HeartGraphNode->GetDefaultInputs())
		{
			CreateInputPin(InputPin);
		}

		for (auto&& OutputPin : HeartGraphNode->GetDefaultInputs())
		{
			CreateOutputPin(OutputPin);
		}
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

			if (SupportsDynamicPins())
			{
				Section.AddMenuEntry(HeartGraphCommands.RefreshContextPins);
			}

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
				Section.AddMenuEntry(HeartGraphCommands.JumpToNodeDefinition);
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
	return SNew(SHeartGraphNode, this);
}

FText UHeartEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (HeartGraphNode)
	{
		return HeartGraphNode->GetNodeTitle();
	}

	return Super::GetNodeTitle(TitleType);
}

FLinearColor UHeartEdGraphNode::GetNodeTitleColor() const
{
	if (HeartGraphNode)
	{
		FLinearColor DynamicColor;
		if (HeartGraphNode->GetDynamicTitleColor(DynamicColor))
		{
			return DynamicColor;
		}
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

UHeartGraphPin* UHeartEdGraphNode::GetPinByName(const FName& Name) const
{
	auto&& HeartPins = GetHeartGraphNode()->GetPinsOfDirection(EHeartPinDirection::Bidirectional);
	for (auto&& Pin : HeartPins)
	{
		if (Pin->GetPinName() == Name)
		{
			return Pin;
		}
	}

	return nullptr;
}

bool UHeartEdGraphNode::CanJumpToDefinition() const
{
	return HeartGraphNode != nullptr;
}

void UHeartEdGraphNode::JumpToDefinition() const
{
	if (HeartGraphNode)
	{
		if (HeartGraphNode->GetClass()->IsNative())
		{
			if (FSourceCodeNavigation::CanNavigateToClass(HeartGraphNode->GetClass()))
			{
				const bool bSucceeded = FSourceCodeNavigation::NavigateToClass(HeartGraphNode->GetClass());
				if (bSucceeded)
				{
					return;
				}
			}

			// Failing that, fall back to the older method which will still get the file open assuming it exists
			FString NativeParentClassHeaderPath;
			const bool bFileFound = FSourceCodeNavigation::FindClassHeaderPath(HeartGraphNode->GetClass(), NativeParentClassHeaderPath) && (IFileManager::Get().FileSize(*NativeParentClassHeaderPath) != INDEX_NONE);
			if (bFileFound)
			{
				const FString AbsNativeParentClassHeaderPath = FPaths::ConvertRelativePathToFull(NativeParentClassHeaderPath);
				FSourceCodeNavigation::OpenSourceFile(AbsNativeParentClassHeaderPath);
			}
		}
		else
		{
			FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(HeartGraphNode->GetClass());
		}
	}
}

void UHeartEdGraphNode::CreateInputPin(const UHeartGraphPin* HeartPin, const int32 Index /*= INDEX_NONE*/)
{
	if (!ensure(IsValid(HeartPin)))
	{
		return;
	}

	UEdGraphPin* NewPin = CreatePin(EGPD_Input, HeartPin->GetPinType(), HeartPin->GetPinName(), Index);
	check(NewPin);

	const FText PinFriendlyName = HeartPin->GetFriendlyName();

	if (!PinFriendlyName.IsEmpty())
	{
		NewPin->bAllowFriendlyName = true;
		NewPin->PinFriendlyName = PinFriendlyName;
	}

	NewPin->PinToolTip = HeartPin->GetToolTip().ToString();

	InputPins.Emplace(NewPin);
}

void UHeartEdGraphNode::CreateOutputPin(const UHeartGraphPin* HeartPin, const int32 Index /*= INDEX_NONE*/)
{
	if (!ensure(IsValid(HeartPin)))
	{
		return;
	}

	UEdGraphPin* NewPin = CreatePin(EGPD_Output, HeartPin->GetPinType(), HeartPin->GetPinName(), Index);
	check(NewPin);

	const FText PinFriendlyName = HeartPin->GetFriendlyName();

	if (!PinFriendlyName.IsEmpty())
	{
		NewPin->bAllowFriendlyName = true;
		NewPin->PinFriendlyName = PinFriendlyName;
	}

	NewPin->PinToolTip = HeartPin->GetToolTip().ToString();

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

bool UHeartEdGraphNode::SupportsDynamicPins() const
{
	return HeartGraphNode && HeartGraphNode->SupportsDynamicPins();
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

	auto&& DefaultPins = HeartGraphNode->GetClass()->GetDefaultObject<UHeartGraphNode>()->GetDefaultInputs();

	// Don't allow user to delete a default pin.
	for (auto&& DefaultPin : DefaultPins)
	{
		if (DefaultPin->GetPinName() == Pin->PinName)
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

	auto&& DefaultPins = HeartGraphNode->GetClass()->GetDefaultObject<UHeartGraphNode>()->GetDefaultOutputs();

	// Don't allow user to delete a default pin.
	for (auto&& DefaultPin : DefaultPins)
	{
		if (DefaultPin->GetPinName() == Pin->PinName)
		{
			return false;
		}
	}

	return true;}

void UHeartEdGraphNode::AddUserInput()
{
	AddInstancePin(EGPD_Input, HeartGraphNode->GetUserInputNum());
}

void UHeartEdGraphNode::AddUserOutput()
{
	AddInstancePin(EGPD_Output, HeartGraphNode->GetUserOutputNum());
}

void UHeartEdGraphNode::AddInstancePin(const EEdGraphPinDirection Direction, const uint8 NumberedPinsAmount)
{
	const FScopedTransaction Transaction(LOCTEXT("AddInstancePin", "Add Instance Pin"));
	Modify();

	UHeartGraphPin* InstancedPin = DuplicateObject(HeartGraphNode->GetInstancedPin(), HeartGraphNode);
	InstancedPin->PinName = *FString::FromInt(NumberedPinsAmount);
	InstancedPin->PinDirection = Direction == EGPD_Input ? EHeartPinDirection::Input : EHeartPinDirection::Output;

	if (Direction == EGPD_Input)
	{
		HeartGraphNode->GetOutputPins().Add(InstancedPin);
		CreateInputPin(InstancedPin, NumberedPinsAmount);
	}
	else
	{
		HeartGraphNode->GetOutputPins().Add(InstancedPin);
		CreateOutputPin(InstancedPin, HeartGraphNode->GetInputPins().Num() + NumberedPinsAmount);
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
			HeartGraphNode->RemoveUserInput(Pin->PinName);

			Pin->MarkAsGarbage();
			Pins.Remove(Pin);
		}
	}
	else
	{
		if (OutputPins.Contains(Pin))
		{
			OutputPins.Remove(Pin);
			HeartGraphNode->RemoveUserOutput(Pin->PinName);

			Pin->MarkAsGarbage();
			Pins.Remove(Pin);
		}
	}

	ReconstructNode();
	GetGraph()->NotifyGraphChanged();
}

void UHeartEdGraphNode::RefreshDynamicPins(const bool bReconstructNode)
{
	if (SupportsDynamicPins())
	{
		const FScopedTransaction Transaction(LOCTEXT("RefreshDynamicPins", "Refresh Context Pins"));
		Modify();

		auto&& TemplateInputs = HeartGraphNode->GetDefaultInputs();
		TemplateInputs.Append(HeartGraphNode->GetDynamicInputs());

		auto&& TemplateOutputs = HeartGraphNode->GetDefaultOutputs();
		TemplateOutputs.Append(HeartGraphNode->GetDynamicOutputs());

		// recreate inputs
		for (auto&& TemplateInput : TemplateInputs)
		{
			auto&& NewPin = DuplicateObject(TemplateInput, HeartGraphNode);
			HeartGraphNode->AddPin(NewPin);
		}

		// recreate outputs
		for (auto&& TemplateInput : TemplateInputs)
		{
			auto&& NewPin = DuplicateObject(TemplateInput, HeartGraphNode);
			HeartGraphNode->AddPin(NewPin);
		}

		if (bReconstructNode)
		{
			ReconstructNode();
			GetGraph()->NotifyGraphChanged();
		}
	}
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
