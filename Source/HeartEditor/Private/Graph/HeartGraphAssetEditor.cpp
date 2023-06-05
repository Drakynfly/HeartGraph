// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartGraphAssetEditor.h"

#include "Graph/HeartGraphAssetToolbar.h"
#include "Graph/HeartEdGraph.h"
#include "Graph/HeartEdGraphSchema.h"
#include "Graph/HeartEdGraphSchema_Actions.h"
#include "Nodes/HeartEdGraphNode.h"

#include "Graph/Widgets/SHeartPalette.h"
#include "Graph/Widgets/SHeartDetailsPanel.h"

#include "Graph/HeartGraphAssetEditorMode_Editor.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

#include "HeartEditorCommands.h"

#include "EdGraphUtilities.h"
#include "EdGraph/EdGraphNode.h"
#include "Editor.h"
#include "EditorClassUtils.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditor.h"
#include "GraphEditorActions.h"
#include "HAL/PlatformApplicationMisc.h"
#include "IDetailsView.h"
#include "PersonaModule.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "ScopedTransaction.h"
#include "SNodePanel.h"
#include "TabSpawners.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "HeartAssetEditor"

namespace Heart::AssetEditor
{
	static const FName AppIdentifier(TEXTVIEW("HeartEditorApp"));

	namespace Modes
	{
		const FName Editor("Heart_AssetEditorMode_Editor");
	}

	FAssetEditor::FAssetEditor()
		: HeartGraph(nullptr)
	{
	}

	FAssetEditor::~FAssetEditor()
	{
		GEditor->UnregisterForUndo(this);
	}

	void FAssetEditor::AddReferencedObjects(FReferenceCollector& Collector)
	{
		Collector.AddReferencedObject(HeartGraph);
	}

	void FAssetEditor::PostUndo(bool bSuccess)
	{
		HandleUndoTransaction();
	}

	void FAssetEditor::PostRedo(bool bSuccess)
	{
		HandleUndoTransaction();
	}

	void FAssetEditor::HandleUndoTransaction()
	{
		SetUISelectionState(NAME_None);
		GraphEditor->NotifyGraphChanged();
		FSlateApplication::Get().DismissAllMenus();
	}

	void FAssetEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
	{
		if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			GraphEditor->NotifyGraphChanged();
		}
	}

	FName FAssetEditor::GetToolkitFName() const
	{
		return FName("HeartEditor");
	}

	FText FAssetEditor::GetBaseToolkitName() const
	{
		return LOCTEXT("AppLabel", "HeartGraph Editor");
	}

	FString FAssetEditor::GetWorldCentricTabPrefix() const
	{
		return LOCTEXT("WorldCentricTabPrefix", "HeartGraph").ToString();
	}

	FLinearColor FAssetEditor::GetWorldCentricTabColorScale() const
	{
		return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
	}

	void FAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
	{
		WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_HeartGraphAssetEditor", "HeartGraph Editor"));

		FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	}

	void FAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
	{
		FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	}

	void FAssetEditor::PostRegenerateMenusAndToolbars()
	{
		// Provide a hyperlink to view our class
		const TSharedRef<SHorizontalBox> MenuOverlayBox = SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				.ShadowOffset(FVector2D::UnitVector)
				.Text(LOCTEXT("HeartGraphAssetEditor_AssetType", "Asset Type: "))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 8.0f, 0.0f)
			[
				FEditorClassUtils::GetSourceLink(HeartGraph->GetClass())
			];

		SetMenuOverlay(MenuOverlayBox);
	}

	void FAssetEditor::SetCurrentMode(const FName NewMode)
	{
		// Clear the selection state when the mode changes.
		SetUISelectionState(NAME_None);

		FWorkflowCentricApplication::SetCurrentMode(NewMode);
	}

	void FAssetEditor::InitAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UHeartGraph* InHeartGraph)
	{
		check(IsValid(InHeartGraph))
		HeartGraph = InHeartGraph;

		// Support undo/redo
		HeartGraph->SetFlags(RF_Transactional);
		GEditor->RegisterForUndo(this);

		BindToolbarCommands();
		CreateToolbar();

		BindGraphCommands();

		constexpr bool bCreateDefaultStandaloneMenu = true;
		constexpr bool bCreateDefaultToolbar = true;
		FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, AppIdentifier, FTabManager::FLayout::NullLayout,
			bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, HeartGraph, false);

		AddApplicationMode(
			Modes::Editor,
			MakeShareable(new FApplicationMode_Editor(SharedThis(this))));

		SetCurrentMode(Modes::Editor);

		//ExtendMenu();
		//ExtendToolbar();

		RegenerateMenusAndToolbars();
	}

	void FAssetEditor::CreateToolbar()
	{
		FName ParentToolbarName;
		const FName ToolBarName = GetToolMenuToolbarName(ParentToolbarName);

		auto&& ToolMenus = UToolMenus::Get();
		auto&& FoundMenu = ToolMenus->FindMenu(ToolBarName);
		if (!FoundMenu || !FoundMenu->IsRegistered())
		{
			FoundMenu = ToolMenus->RegisterMenu(ToolBarName, ParentToolbarName, EMultiBoxType::ToolBar);
		}

		if (FoundMenu)
		{
			AssetToolbar = MakeShareable(new FHeartGraphAssetToolbar(SharedThis(this), FoundMenu));
		}
	}

	void FAssetEditor::BindToolbarCommands()
	{
		FHeartGraphToolbarCommands::Register();
		const FHeartGraphToolbarCommands& ToolbarCommands = FHeartGraphToolbarCommands::Get();

		// Editing
		ToolkitCommands->MapAction(ToolbarCommands.RefreshAsset,
			FExecuteAction::CreateSP(this, &FAssetEditor::RefreshAsset),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanEdit));
	}

	void FAssetEditor::RefreshAsset()
	{
		SetCurrentMode(Modes::Editor);

		/*
		TArray<UHeartGraphNode*> HeartGraphNodes;
		HeartGraph->GetNodeArray(HeartGraphNodes);

		for (auto&& GraphNode : HeartGraphNodes)
		{
			Cast<UHeartEdGraphNode>(GraphNode->GetEdGraphNode())->RefreshDynamicPins(true);
		}
		*/
	}

	FGraphAppearanceInfo FAssetEditor::GetGraphAppearanceInfo() const
	{
		FGraphAppearanceInfo AppearanceInfo;
		AppearanceInfo.CornerText = GetCornerText();
		return AppearanceInfo;
	}

	FText FAssetEditor::GetCornerText() const
	{
		if (ensure(IsValid(HeartGraph)))
		{
			return FText::FromString(HeartGraph->GetGraphTypeName().ToString().ToUpper());
		}

		return LOCTEXT("AppearanceCornerText_HeartGraphErrorText", "UNKNOWN");
	}

	void FAssetEditor::BindGraphCommands()
	{
		FGraphEditorCommands::Register();
		FHeartGraphCommands::Register();
		FHeartSpawnNodeCommands::Register();

		const FGenericCommands& GenericCommands = FGenericCommands::Get();
		const FGraphEditorCommandsImpl& GraphCommands = FGraphEditorCommands::Get();
		const FHeartGraphCommands& HeartGraphCommands = FHeartGraphCommands::Get();

		// Graph commands
		ToolkitCommands->MapAction(GraphCommands.CreateComment,
			FExecuteAction::CreateSP(this, &FAssetEditor::OnCreateComment),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanEdit));

		ToolkitCommands->MapAction(GraphCommands.StraightenConnections,
			FExecuteAction::CreateSP(this, &FAssetEditor::OnStraightenConnections));

		// Generic Node commands
		ToolkitCommands->MapAction(GenericCommands.Undo,
			FExecuteAction::CreateStatic(&FAssetEditor::UndoGraphAction),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanEdit));

		ToolkitCommands->MapAction(GenericCommands.Redo,
			FExecuteAction::CreateStatic(&FAssetEditor::RedoGraphAction),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanEdit));

		ToolkitCommands->MapAction(GenericCommands.SelectAll,
			FExecuteAction::CreateSP(this, &FAssetEditor::SelectAllNodes),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanSelectAllNodes));

		ToolkitCommands->MapAction(GenericCommands.Delete,
			FExecuteAction::CreateSP(this, &FAssetEditor::DeleteSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanDeleteNodes));

		ToolkitCommands->MapAction(GenericCommands.Copy,
			FExecuteAction::CreateSP(this, &FAssetEditor::CopySelectedNodes),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanCopyNodes));

		ToolkitCommands->MapAction(GenericCommands.Cut,
			FExecuteAction::CreateSP(this, &FAssetEditor::CutSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanCutNodes));

		ToolkitCommands->MapAction(GenericCommands.Paste,
			FExecuteAction::CreateSP(this, &FAssetEditor::PasteNodes),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanPasteNodes));

		ToolkitCommands->MapAction(GenericCommands.Duplicate,
			FExecuteAction::CreateSP(this, &FAssetEditor::DuplicateNodes),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanDuplicateNodes));

		// Pin commands
		ToolkitCommands->MapAction(HeartGraphCommands.AddInput,
			FExecuteAction::CreateSP(this, &FAssetEditor::AddInput),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanAddInput));

		ToolkitCommands->MapAction(HeartGraphCommands.AddOutput,
			FExecuteAction::CreateSP(this, &FAssetEditor::AddOutput),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanAddOutput));

		ToolkitCommands->MapAction(HeartGraphCommands.RemovePin,
			FExecuteAction::CreateSP(this, &FAssetEditor::RemovePin),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanRemovePin));

		// Breakpoint commands
		ToolkitCommands->MapAction(GraphCommands.AddBreakpoint,
			FExecuteAction::CreateSP(this, &FAssetEditor::OnAddBreakpoint),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanAddBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FAssetEditor::CanAddBreakpoint)
		);

		ToolkitCommands->MapAction(GraphCommands.RemoveBreakpoint,
			FExecuteAction::CreateSP(this, &FAssetEditor::OnRemoveBreakpoint),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanRemoveBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FAssetEditor::CanRemoveBreakpoint)
		);

		ToolkitCommands->MapAction(GraphCommands.EnableBreakpoint,
			FExecuteAction::CreateSP(this, &FAssetEditor::OnEnableBreakpoint),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanEnableBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FAssetEditor::CanEnableBreakpoint)
		);

		ToolkitCommands->MapAction(GraphCommands.DisableBreakpoint,
			FExecuteAction::CreateSP(this, &FAssetEditor::OnDisableBreakpoint),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanDisableBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FAssetEditor::CanDisableBreakpoint)
		);

		ToolkitCommands->MapAction(GraphCommands.ToggleBreakpoint,
			FExecuteAction::CreateSP(this, &FAssetEditor::OnToggleBreakpoint),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanToggleBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FAssetEditor::CanToggleBreakpoint)
		);

		// Pin Breakpoint commands
		ToolkitCommands->MapAction(HeartGraphCommands.AddPinBreakpoint,
			FExecuteAction::CreateSP(this, &FAssetEditor::OnAddPinBreakpoint),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanAddPinBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FAssetEditor::CanAddPinBreakpoint)
		);

		ToolkitCommands->MapAction(HeartGraphCommands.RemovePinBreakpoint,
			FExecuteAction::CreateSP(this, &FAssetEditor::OnRemovePinBreakpoint),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanRemovePinBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FAssetEditor::CanRemovePinBreakpoint)
		);

		ToolkitCommands->MapAction(HeartGraphCommands.EnablePinBreakpoint,
			FExecuteAction::CreateSP(this, &FAssetEditor::OnEnablePinBreakpoint),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanEnablePinBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FAssetEditor::CanEnablePinBreakpoint)
		);

		ToolkitCommands->MapAction(HeartGraphCommands.DisablePinBreakpoint,
			FExecuteAction::CreateSP(this, &FAssetEditor::OnDisablePinBreakpoint),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanDisablePinBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FAssetEditor::CanDisablePinBreakpoint)
		);

		ToolkitCommands->MapAction(HeartGraphCommands.TogglePinBreakpoint,
			FExecuteAction::CreateSP(this, &FAssetEditor::OnTogglePinBreakpoint),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanTogglePinBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FAssetEditor::CanTogglePinBreakpoint)
		);

		// Jump commands
		ToolkitCommands->MapAction(HeartGraphCommands.JumpToGraphNodeDefinition,
	        FExecuteAction::CreateSP(this, &FAssetEditor::JumpToGraphNodeDefinition),
	        FCanExecuteAction::CreateSP(this, &FAssetEditor::CanJumpToGraphNodeDefinition));

		// Jump commands
		ToolkitCommands->MapAction(HeartGraphCommands.JumpToNodeDefinition,
			FExecuteAction::CreateSP(this, &FAssetEditor::JumpToNodeObjectDefinition),
			FCanExecuteAction::CreateSP(this, &FAssetEditor::CanJumpToNodeObjectDefinition));
	}

	void FAssetEditor::OnDetailsPanelCreated(const TSharedRef<SDetailsPanel, ESPMode::ThreadSafe>& DetailsView)
	{
		DetailsPanel = DetailsView;
	}

	void FAssetEditor::OnNodePaletteCreated(const TSharedRef<SHeartPalette, ESPMode::ThreadSafe>& NodePalette)
	{
		Palette = NodePalette;
	}

	TSharedRef<SGraphEditor> FAssetEditor::CreateGraphWidget(const FWorkflowTabSpawnInfo& Info)
	{
		SGraphEditor::FGraphEditorEvents InEvents;
		InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FAssetEditor::OnSelectedNodesChanged);
		InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FAssetEditor::OnNodeDoubleClicked);
		InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FAssetEditor::OnNodeTitleCommitted);
		InEvents.OnSpawnNodeByShortcut = SGraphEditor::FOnSpawnNodeByShortcut::CreateStatic(&FAssetEditor::OnSpawnGraphNodeByShortcut, HeartGraph->GetEdGraph());

		GraphEditor = SNew(SGraphEditor)
			.AdditionalCommands(ToolkitCommands)
			.IsEditable(true)
			.Appearance(GetGraphAppearanceInfo())
			.GraphToEdit(HeartGraph->GetEdGraph())
			.GraphEvents(InEvents)
			.AutoExpandActionMenu(true)
			.ShowGraphStateOverlay(false);

		return GraphEditor.ToSharedRef();
	}

	void FAssetEditor::UndoGraphAction()
	{
		GEditor->UndoTransaction();
	}

	void FAssetEditor::RedoGraphAction()
	{
		GEditor->RedoTransaction();
	}

	FReply FAssetEditor::OnSpawnGraphNodeByShortcut(FInputChord InChord, const FVector2D& InPosition, UEdGraph* InGraph)
	{
		if (FHeartSpawnNodeCommands::IsRegistered())
		{
			const TSharedPtr<FEdGraphSchemaAction> Action = FHeartSpawnNodeCommands::Get().GetActionByChord(InChord);
	        if (Action.IsValid())
	        {
        		TArray<UEdGraphPin*> DummyPins;
        		Action->PerformAction(InGraph, DummyPins, InPosition);
        		return FReply::Handled();
	        }
		}

		return FReply::Unhandled();
	}

	void FAssetEditor::SetUISelectionState(const FName SelectionOwner)
	{
		if (SelectionOwner != CurrentUISelection)
		{
			ClearSelectionStateFor(CurrentUISelection);
			CurrentUISelection = SelectionOwner;
		}
	}

	void FAssetEditor::ClearSelectionStateFor(const FName SelectionOwner)
	{
		if (SelectionOwner == FGraphEditorSummoner::TabId)
		{
			GraphEditor->ClearSelectionSet();
		}
		else if (SelectionOwner == FNodePaletteSummoner::TabId)
		{
			if (Palette.IsValid())
			{
				Palette->ClearGraphActionMenuSelection();
			}
		}
	}

	void FAssetEditor::OnCreateComment() const
	{
		FHeartGraphSchemaAction_NewComment CommentAction;
		CommentAction.PerformAction(HeartGraph->GetEdGraph(), nullptr, GraphEditor->GetPasteLocation());
	}

	void FAssetEditor::OnStraightenConnections() const
	{
		GraphEditor->OnStraightenConnections();
	}

	bool FAssetEditor::CanEdit() const
	{
		return GEditor->PlayWorld == nullptr;
	}

	EVisibility FAssetEditor::GetDebuggerVisibility()
	{
		return GEditor->PlayWorld ? EVisibility::Visible : EVisibility::Collapsed;
	}

	TSet<UHeartEdGraphNode*> FAssetEditor::GetSelectedHeartGraphNodes() const
	{
		TSet<UHeartEdGraphNode*> Result;

		const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			if (auto&& SelectedNode = Cast<UHeartEdGraphNode>(*NodeIt))
			{
				Result.Emplace(SelectedNode);
			}
		}

		return Result;
	}

	int32 FAssetEditor::GetNumberOfSelectedNodes() const
	{
		return GraphEditor->GetSelectedNodes().Num();
	}

	bool FAssetEditor::GetBoundsForSelectedNodes(FSlateRect& Rect, const float Padding) const
	{
		return GraphEditor->GetBoundsForSelectedNodes(Rect, Padding);
	}

	void FAssetEditor::OnSelectedNodesChanged(const TSet<UObject*>& Nodes)
	{
		TArray<UObject*> SelectedGraphObjects;
		TArray<UObject*> SelectedObjects;

		if (Nodes.Num() > 0)
		{
			SetUISelectionState(FGraphEditorSummoner::TabId);

			for (TSet<UObject*>::TConstIterator SetIt(Nodes); SetIt; ++SetIt)
			{
				if (auto&& GraphNode = Cast<UHeartEdGraphNode>(*SetIt))
				{
					SelectedGraphObjects.Add(GraphNode->GetHeartGraphNode());
					SelectedObjects.Add(GraphNode->GetHeartGraphNode()->GetNodeObject());
				}
				else
				{
					SelectedGraphObjects.Add(*SetIt);
				}
			}
		}
		else
		{
			SetUISelectionState(NAME_None);
			SelectedGraphObjects.Add(GetHeartGraph());
		}

		if (DetailsPanel.IsValid())
		{
			DetailsPanel->DetailsView_Graph->SetObjects(SelectedGraphObjects);
		}

		if (DetailsPanel.IsValid())
		{
			DetailsPanel->DetailsView_Object->SetObjects(SelectedObjects);
		}
	}

	void FAssetEditor::SelectSingleNode(UEdGraphNode* Node) const
	{
		if (GraphEditor)
		{
			GraphEditor->ClearSelectionSet();
	        GraphEditor->SetNodeSelection(Node, true);
		}

	}

	void FAssetEditor::SelectAllNodes() const
	{
		if (GraphEditor)
		{
			GraphEditor->SelectAllNodes();
		}
	}

	bool FAssetEditor::CanSelectAllNodes() const
	{
		return true;
	}

	void FAssetEditor::DeleteNode(UEdGraphNode* Node)
	{
		check(Node);

		const UEdGraphSchema* Schema = nullptr;

		// Ensure we mark parent graph modified
		if (UEdGraph* GraphObj = Node->GetGraph())
		{
			GraphObj->Modify();
			Schema = GraphObj->GetSchema();
		}

		Node->Modify();

		if (Schema)
		{
			Schema->BreakNodeLinks(*Node);
		}

		// Try to remove the Runtime Node first, if there is one.
		if (auto&& HeartEdGraphNode = Cast<UHeartEdGraphNode>(Node))
		{
			if (auto&& RuntimeNode = HeartEdGraphNode->GetHeartGraphNode())
			{
				HeartGraph->RemoveNode(RuntimeNode->GetGuid());
			}
		}

		// Destroy Editor node
		Node->DestroyNode();
	}

	void FAssetEditor::DeleteSelectedNodes()
	{
		const FScopedTransaction Transaction(LOCTEXT("DeleteSelectedNode", "Delete Selected Node"));
		GraphEditor->GetCurrentGraph()->Modify();
		HeartGraph->Modify();

		const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
		SetUISelectionState(NAME_None);

		for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			auto&& Node = CastChecked<UEdGraphNode>(*NodeIt);

			if (Node->CanUserDeleteNode())
			{
				DeleteNode(Node);
			}
		}
	}

	void FAssetEditor::DeleteSelectedDuplicableNodes()
	{
		// Cache off the old selection
		const FGraphPanelSelectionSet OldSelectedNodes = GraphEditor->GetSelectedNodes();

		// Clear the selection and only select the nodes that can be duplicated
		FGraphPanelSelectionSet RemainingNodes;
		GraphEditor->ClearSelectionSet();

		for (FGraphPanelSelectionSet::TConstIterator SelectedIt(OldSelectedNodes); SelectedIt; ++SelectedIt)
		{
			if (auto&& Node = Cast<UEdGraphNode>(*SelectedIt))
			{
				if (Node->CanDuplicateNode())
				{
					GraphEditor->SetNodeSelection(Node, true);
				}
				else
				{
					RemainingNodes.Add(Node);
				}
			}
		}

		// Delete the duplicable nodes
		DeleteSelectedNodes();

		for (FGraphPanelSelectionSet::TConstIterator SelectedIt(RemainingNodes); SelectedIt; ++SelectedIt)
		{
			if (auto&& Node = Cast<UEdGraphNode>(*SelectedIt))
			{
				GraphEditor->SetNodeSelection(Node, true);
			}
		}
	}

	bool FAssetEditor::CanDeleteNodes() const
	{
		if (CanEdit())
		{
			const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
			for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
			{
				if (auto&& Node = Cast<UEdGraphNode>(*NodeIt))
				{
					if (!Node->CanUserDeleteNode())
					{
						return false;
					}
				}
			}

			return SelectedNodes.Num() > 0;
		}

		return false;
	}

	void FAssetEditor::CutSelectedNodes()
	{
		CopySelectedNodes();

		// Cut should only delete nodes that can be duplicated
		DeleteSelectedDuplicableNodes();
	}

	bool FAssetEditor::CanCutNodes() const
	{
		return CanCopyNodes() && CanDeleteNodes();
	}

	void FAssetEditor::CopySelectedNodes() const
	{
		const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator SelectedIt(SelectedNodes); SelectedIt; ++SelectedIt)
		{
			if (auto&& Node = Cast<UHeartEdGraphNode>(*SelectedIt))
			{
				Node->PrepareForCopying();
			}
		}

		// Export the selected nodes and place the text on the clipboard
		FString ExportedText;
		FEdGraphUtilities::ExportNodesToText(SelectedNodes, /*out*/ ExportedText);
		FPlatformApplicationMisc::ClipboardCopy(*ExportedText);

		for (FGraphPanelSelectionSet::TConstIterator SelectedIt(SelectedNodes); SelectedIt; ++SelectedIt)
		{
			if (auto&& Node = Cast<UHeartEdGraphNode>(*SelectedIt))
			{
				Node->PostCopyNode();
			}
		}
	}

	bool FAssetEditor::CanCopyNodes() const
	{
		if (CanEdit())
		{
			const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
			for (FGraphPanelSelectionSet::TConstIterator SelectedIt(SelectedNodes); SelectedIt; ++SelectedIt)
			{
				auto&& Node = Cast<UEdGraphNode>(*SelectedIt);
				if (Node && Node->CanDuplicateNode())
				{
					return true;
				}
			}
		}

		return false;
	}

	void FAssetEditor::PasteNodes()
	{
		PasteNodesHere(GraphEditor->GetPasteLocation());
	}

	void FAssetEditor::PasteNodesHere(const FVector2D& Location)
	{
		SetUISelectionState(NAME_None);

		// Undo/Redo support
		const FScopedTransaction Transaction(LOCTEXT("PasteNode", "Paste Node"));
		HeartGraph->GetEdGraph()->Modify();
		HeartGraph->Modify();

		// Clear the selection set (newly pasted stuff will be selected)
		GraphEditor->ClearSelectionSet();

		// Grab the text to paste from the clipboard.
		FString TextToImport;
		FPlatformApplicationMisc::ClipboardPaste(TextToImport);

		// Import the nodes
		TSet<UEdGraphNode*> PastedNodes;
		FEdGraphUtilities::ImportNodesFromText(HeartGraph->GetEdGraph(), TextToImport, /*out*/ PastedNodes);

		//Average position of nodes so we can move them while still maintaining relative distances to each other
		FVector2D AvgNodePosition(0.0f, 0.0f);

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			auto&& Node = *It;
			AvgNodePosition.X += Node->NodePosX;
			AvgNodePosition.Y += Node->NodePosY;
		}

		if (PastedNodes.Num() > 0)
		{
			const float InvNumNodes = 1.0f / static_cast<float>(PastedNodes.Num());
			AvgNodePosition.X *= InvNumNodes;
			AvgNodePosition.Y *= InvNumNodes;
		}

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			auto&& Node = *It;

			// Give new node a different Guid from the old one
			Node->CreateNewGuid();

			if (auto&& EdGraphNode = Cast<UHeartEdGraphNode>(Node))
			{
				HeartGraph->AddNode(EdGraphNode->GetHeartGraphNode());
			}

			// Select the newly pasted stuff
			GraphEditor->SetNodeSelection(Node, true);

			Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
			Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

			Node->SnapToGrid(SNodePanel::GetSnapGridSize());
		}

		// @todo probably broadcast something here instead, or handle in child implementation
		//HeartGraph->HarvestNodeConnections();

		// Update UI
		GraphEditor->NotifyGraphChanged();

		HeartGraph->PostEditChange();
		HeartGraph->MarkPackageDirty();
	}

	bool FAssetEditor::CanPasteNodes() const
	{
		if (CanEdit())
		{
			FString ClipboardContent;
			FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

			return FEdGraphUtilities::CanImportNodesFromText(HeartGraph->GetEdGraph(), ClipboardContent);
		}

		return false;
	}

	void FAssetEditor::DuplicateNodes()
	{
		CopySelectedNodes();
		PasteNodes();
	}

	bool FAssetEditor::CanDuplicateNodes() const
	{
		return CanCopyNodes();
	}

	void FAssetEditor::OnNodeDoubleClicked(UEdGraphNode* Node) const
	{
		if (auto&& HeartGraphNode = Cast<UHeartEdGraphNode>(Node)->GetHeartGraphNode())
		{
			Node->JumpToDefinition();
		}
	}

	void FAssetEditor::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged)
	{
		if (NodeBeingChanged)
		{
			const FScopedTransaction Transaction(LOCTEXT("RenameNode", "Rename Node"));
			NodeBeingChanged->Modify();
			NodeBeingChanged->OnRenameNode(NewText.ToString());
		}
	}

	void FAssetEditor::AddInput() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->AddUserInput();
		}
	}

	bool FAssetEditor::CanAddInput() const
	{
		if (CanEdit() && GetSelectedHeartGraphNodes().Num() == 1)
		{
			for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
			{
				return SelectedNode->CanUserAddInput();
			}
		}

		return false;
	}

	void FAssetEditor::AddOutput() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->AddUserOutput();
		}
	}

	bool FAssetEditor::CanAddOutput() const
	{
		if (CanEdit() && GetSelectedHeartGraphNodes().Num() == 1)
		{
			for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
			{
				return SelectedNode->CanUserAddOutput();
			}
		}

		return false;
	}

	void FAssetEditor::RemovePin() const
	{
		if (auto&& SelectedPin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& SelectedNode = Cast<UHeartEdGraphNode>(SelectedPin->GetOwningNode()))
			{
				SelectedNode->RemoveInstancePin(SelectedPin);
			}
		}
	}

	bool FAssetEditor::CanRemovePin() const
	{
		if (CanEdit() && GetSelectedHeartGraphNodes().Num() == 1)
		{
			if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
			{
				if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
				{
					if (Pin->Direction == EGPD_Input)
					{
						return GraphNode->CanUserRemoveInput(Pin);
					}
					else
					{
						return GraphNode->CanUserRemoveOutput(Pin);
					}
				}
			}
		}

		return false;
	}

	void FAssetEditor::OnAddBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->NodeBreakpoint.AddBreakpoint();
		}
	}

	void FAssetEditor::OnAddPinBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				GraphNode->PinBreakpoints.Add(Pin, FHeartBreakpoint());
				GraphNode->PinBreakpoints[Pin].AddBreakpoint();
			}
		}
	}

	bool FAssetEditor::CanAddBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			return !SelectedNode->NodeBreakpoint.HasBreakpoint();
		}

		return false;
	}

	bool FAssetEditor::CanAddPinBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				return !GraphNode->PinBreakpoints.Contains(Pin) || !GraphNode->PinBreakpoints[Pin].HasBreakpoint();
			}
		}

		return false;
	}

	void FAssetEditor::OnRemoveBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->NodeBreakpoint.RemoveBreakpoint();
		}
	}

	void FAssetEditor::OnRemovePinBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				GraphNode->PinBreakpoints.Remove(Pin);
			}
		}
	}

	bool FAssetEditor::CanRemoveBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			return SelectedNode->NodeBreakpoint.HasBreakpoint();
		}

		return false;
	}

	bool FAssetEditor::CanRemovePinBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				return GraphNode->PinBreakpoints.Contains(Pin);
			}
		}

		return false;
	}

	void FAssetEditor::OnEnableBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->NodeBreakpoint.EnableBreakpoint();
		}
	}

	void FAssetEditor::OnEnablePinBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				GraphNode->PinBreakpoints[Pin].EnableBreakpoint();
			}
		}
	}

	bool FAssetEditor::CanEnableBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				return GraphNode->PinBreakpoints.Contains(Pin);
			}
		}

		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			return SelectedNode->NodeBreakpoint.CanEnableBreakpoint();
		}

		return false;
	}

	bool FAssetEditor::CanEnablePinBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				return GraphNode->PinBreakpoints.Contains(Pin) && GraphNode->PinBreakpoints[Pin].CanEnableBreakpoint();
			}
		}

		return false;
	}

	void FAssetEditor::OnDisableBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->NodeBreakpoint.DisableBreakpoint();
		}
	}

	void FAssetEditor::OnDisablePinBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				GraphNode->PinBreakpoints[Pin].DisableBreakpoint();
			}
		}
	}

	bool FAssetEditor::CanDisableBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			return SelectedNode->NodeBreakpoint.IsBreakpointEnabled();
		}

		return false;
	}

	bool FAssetEditor::CanDisablePinBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				return GraphNode->PinBreakpoints.Contains(Pin) && GraphNode->PinBreakpoints[Pin].IsBreakpointEnabled();
			}
		}

		return false;
	}

	void FAssetEditor::OnToggleBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->NodeBreakpoint.ToggleBreakpoint();
		}
	}

	void FAssetEditor::OnTogglePinBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				GraphNode->PinBreakpoints.Add(Pin, FHeartBreakpoint());
				GraphNode->PinBreakpoints[Pin].ToggleBreakpoint();
			}
		}
	}

	bool FAssetEditor::CanToggleBreakpoint() const
	{
		return GetSelectedHeartGraphNodes().Num() > 0;
	}

	bool FAssetEditor::CanTogglePinBreakpoint() const
	{
		return GraphEditor->GetGraphPinForMenu() != nullptr;
	}

	void FAssetEditor::JumpToGraphNodeDefinition() const
	{
		// Iterator used but should only contain one node
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->JumpToDefinition();
			return;
		}
	}

	bool FAssetEditor::CanJumpToGraphNodeDefinition() const
	{
		if (!GetSelectedHeartGraphNodes().Num() == 1)
		{
			return false;
		}

		// Iterator used but should only contain one node
		for (auto&& EdGraphNode : GetSelectedHeartGraphNodes())
		{
			return EdGraphNode && IsValid(EdGraphNode->GetHeartGraphNode());
		}

		return false;
	}

	void FAssetEditor::JumpToNodeObjectDefinition() const
	{
		// Iterator used but should only contain one node
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->JumpToNodeDefinition();
			return;
		}
	}

	bool FAssetEditor::CanJumpToNodeObjectDefinition() const
	{
		if (!GetSelectedHeartGraphNodes().Num() == 1)
	    {
    		return false;
	    }

		// Iterator used but should only contain one node
		for (auto&& EdGraphNode : GetSelectedHeartGraphNodes())
		{
			return EdGraphNode && IsValid(EdGraphNode->GetHeartGraphNode())
				&& IsValid(EdGraphNode->GetHeartGraphNode()->GetNodeObject());
		}

	    return false;
	}
}

#undef LOCTEXT_NAMESPACE
