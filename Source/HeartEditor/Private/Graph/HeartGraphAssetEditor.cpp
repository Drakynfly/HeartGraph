// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartGraphAssetEditor.h"

#include "Graph/HeartGraphAssetToolbar.h"
#include "Graph/HeartEdGraphSchema_Actions.h"
#include "Nodes/HeartEdGraphNode.h"

#include "Graph/Widgets/SHeartPalette.h"
#include "Graph/Widgets/SHeartDetailsPanel.h"

#include "AssetEditor/ApplicationMode_Editor.h"
#include "AssetEditor/TabSpawners.h"

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
#include "HeartEditorModule.h"
#include "HAL/PlatformApplicationMisc.h"
#include "IDetailsView.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "ScopedTransaction.h"
#include "SNodePanel.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "HeartAssetEditor"

namespace Heart::AssetEditor
{
	static const FName AppIdentifier(TEXTVIEW("HeartEditorApp"));

	FHeartGraphEditor::FHeartGraphEditor()
		: HeartGraph(nullptr)
	{
	}

	FHeartGraphEditor::~FHeartGraphEditor()
	{
		GEditor->UnregisterForUndo(this);
	}

	void FHeartGraphEditor::AddReferencedObjects(FReferenceCollector& Collector)
	{
		Collector.AddReferencedObject(HeartGraph);
	}

	void FHeartGraphEditor::PostUndo(bool bSuccess)
	{
		HandleUndoTransaction();
	}

	void FHeartGraphEditor::PostRedo(bool bSuccess)
	{
		HandleUndoTransaction();
	}

	void FHeartGraphEditor::HandleUndoTransaction()
	{
		SetUISelectionState(NAME_None);
		if (GraphEditor)
		{
			GraphEditor->NotifyGraphChanged();
		}
		FSlateApplication::Get().DismissAllMenus();
	}

	void FHeartGraphEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
	{
		if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			GraphEditor->NotifyGraphChanged();
		}
	}

	FName FHeartGraphEditor::GetToolkitFName() const
	{
		return FName("HeartEditor");
	}

	FText FHeartGraphEditor::GetBaseToolkitName() const
	{
		return LOCTEXT("AppLabel", "HeartGraph Editor");
	}

	FString FHeartGraphEditor::GetWorldCentricTabPrefix() const
	{
		return LOCTEXT("WorldCentricTabPrefix", "HeartGraph").ToString();
	}

	FLinearColor FHeartGraphEditor::GetWorldCentricTabColorScale() const
	{
		return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
	}

	void FHeartGraphEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
	{
		WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_HeartGraphAssetEditor", "HeartGraph Editor"));

		FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	}

	void FHeartGraphEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
	{
		FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	}

	void FHeartGraphEditor::PostRegenerateMenusAndToolbars()
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

	void FHeartGraphEditor::SetCurrentMode(const FName NewMode)
	{
		// Clear the selection state when the mode changes.
		SetUISelectionState(NAME_None);

		FWorkflowCentricApplication::SetCurrentMode(NewMode);
	}

	void FHeartGraphEditor::InitAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UHeartGraph* InHeartGraph)
	{
		check(IsValid(InHeartGraph))
		HeartGraph = InHeartGraph;

		// Support undo/redo
		HeartGraph->SetFlags(RF_Transactional);
		GEditor->RegisterForUndo(this);

		BindToolbarCommands();
		BindGraphCommands();

		constexpr bool bCreateDefaultStandaloneMenu = true;
		constexpr bool bCreateDefaultToolbar = true;
		FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, AppIdentifier, FTabManager::FLayout::NullLayout,
			bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, HeartGraph, false);

		// Toolbar must be initialized before Application Modes are added, as they will probably use the toolbar.
		CreateToolbar();

		FHeartEditorModule& HeartEditorModule = FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");

		// Add all registered application modes.
		for (auto&& ModeData : HeartEditorModule.GetApplicationModes())
		{
			AddApplicationMode(
				ModeData.Key,
				ModeData.Value.CreateModeInstance.Execute(SharedThis(this)));
		}

		// Default mode to open on
		SetCurrentMode(FApplicationMode_Editor::ModeID);

		//ExtendMenu();
		//ExtendToolbar();

		AddMenuExtender(HeartEditorModule.GetMenuExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));
		AddToolbarExtender(HeartEditorModule.GetToolBarExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));

		RegenerateMenusAndToolbars();
	}

	bool FHeartGraphEditor::CanActivateMode(FName NewMode) const
	{
		// @todo
		return true;
	}

	void FHeartGraphEditor::CreateToolbar()
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

	void FHeartGraphEditor::BindToolbarCommands()
	{
		FHeartGraphToolbarCommands::Register();
		const FHeartGraphToolbarCommands& ToolbarCommands = FHeartGraphToolbarCommands::Get();

		// Editing
		ToolkitCommands->MapAction(ToolbarCommands.RefreshAsset,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::RefreshAsset),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanEdit));
	}

	void FHeartGraphEditor::RefreshAsset()
	{
		/*
		TArray<UHeartGraphNode*> HeartGraphNodes;
		HeartGraph->GetNodeArray(HeartGraphNodes);

		for (auto&& GraphNode : HeartGraphNodes)
		{
			Cast<UHeartEdGraphNode>(GraphNode->GetEdGraphNode())->RefreshDynamicPins(true);
		}
		*/
	}

	FGraphAppearanceInfo FHeartGraphEditor::GetGraphAppearanceInfo() const
	{
		FGraphAppearanceInfo AppearanceInfo;
		AppearanceInfo.CornerText = GetCornerText();
		return AppearanceInfo;
	}

	FText FHeartGraphEditor::GetCornerText() const
	{
		if (ensure(IsValid(HeartGraph)))
		{
			return FText::FromString(HeartGraph->GetGraphTypeName().ToString().ToUpper());
		}

		return LOCTEXT("AppearanceCornerText_HeartGraphErrorText", "UNKNOWN");
	}

	void FHeartGraphEditor::BindGraphCommands()
	{
		FGraphEditorCommands::Register();
		FHeartGraphCommands::Register();
		FHeartSpawnNodeCommands::Register();

		const FGenericCommands& GenericCommands = FGenericCommands::Get();
		const FGraphEditorCommandsImpl& GraphCommands = FGraphEditorCommands::Get();
		const FHeartGraphCommands& HeartGraphCommands = FHeartGraphCommands::Get();

		// Graph commands
		ToolkitCommands->MapAction(GraphCommands.CreateComment,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::OnCreateComment),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanEdit));

		ToolkitCommands->MapAction(GraphCommands.StraightenConnections,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::OnStraightenConnections));

		// Generic Node commands
		ToolkitCommands->MapAction(GenericCommands.Undo,
			FExecuteAction::CreateStatic(&FHeartGraphEditor::UndoGraphAction),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanEdit));

		ToolkitCommands->MapAction(GenericCommands.Redo,
			FExecuteAction::CreateStatic(&FHeartGraphEditor::RedoGraphAction),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanEdit));

		ToolkitCommands->MapAction(GenericCommands.SelectAll,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::SelectAllNodes),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanSelectAllNodes));

		ToolkitCommands->MapAction(GenericCommands.Delete,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::DeleteSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanDeleteNodes));

		ToolkitCommands->MapAction(GenericCommands.Copy,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::CopySelectedNodes),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanCopyNodes));

		ToolkitCommands->MapAction(GenericCommands.Cut,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::CutSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanCutNodes));

		ToolkitCommands->MapAction(GenericCommands.Paste,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::PasteNodes),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanPasteNodes));

		ToolkitCommands->MapAction(GenericCommands.Duplicate,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::DuplicateNodes),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanDuplicateNodes));

		// Pin commands
		ToolkitCommands->MapAction(HeartGraphCommands.AddInput,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::AddInput),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanAddInput));

		ToolkitCommands->MapAction(HeartGraphCommands.AddOutput,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::AddOutput),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanAddOutput));

		ToolkitCommands->MapAction(HeartGraphCommands.RemovePin,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::RemovePin),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanRemovePin));

		// Breakpoint commands
		ToolkitCommands->MapAction(GraphCommands.AddBreakpoint,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::OnAddBreakpoint),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanAddBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FHeartGraphEditor::CanAddBreakpoint)
		);

		ToolkitCommands->MapAction(GraphCommands.RemoveBreakpoint,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::OnRemoveBreakpoint),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanRemoveBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FHeartGraphEditor::CanRemoveBreakpoint)
		);

		ToolkitCommands->MapAction(GraphCommands.EnableBreakpoint,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::OnEnableBreakpoint),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanEnableBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FHeartGraphEditor::CanEnableBreakpoint)
		);

		ToolkitCommands->MapAction(GraphCommands.DisableBreakpoint,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::OnDisableBreakpoint),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanDisableBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FHeartGraphEditor::CanDisableBreakpoint)
		);

		ToolkitCommands->MapAction(GraphCommands.ToggleBreakpoint,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::OnToggleBreakpoint),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanToggleBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FHeartGraphEditor::CanToggleBreakpoint)
		);

		// Pin Breakpoint commands
		ToolkitCommands->MapAction(HeartGraphCommands.AddPinBreakpoint,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::OnAddPinBreakpoint),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanAddPinBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FHeartGraphEditor::CanAddPinBreakpoint)
		);

		ToolkitCommands->MapAction(HeartGraphCommands.RemovePinBreakpoint,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::OnRemovePinBreakpoint),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanRemovePinBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FHeartGraphEditor::CanRemovePinBreakpoint)
		);

		ToolkitCommands->MapAction(HeartGraphCommands.EnablePinBreakpoint,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::OnEnablePinBreakpoint),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanEnablePinBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FHeartGraphEditor::CanEnablePinBreakpoint)
		);

		ToolkitCommands->MapAction(HeartGraphCommands.DisablePinBreakpoint,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::OnDisablePinBreakpoint),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanDisablePinBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FHeartGraphEditor::CanDisablePinBreakpoint)
		);

		ToolkitCommands->MapAction(HeartGraphCommands.TogglePinBreakpoint,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::OnTogglePinBreakpoint),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanTogglePinBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FHeartGraphEditor::CanTogglePinBreakpoint)
		);

		// Jump commands
		ToolkitCommands->MapAction(HeartGraphCommands.JumpToGraphNodeDefinition,
	        FExecuteAction::CreateSP(this, &FHeartGraphEditor::JumpToGraphNodeDefinition),
	        FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanJumpToGraphNodeDefinition));

		// Jump commands
		ToolkitCommands->MapAction(HeartGraphCommands.JumpToNodeDefinition,
			FExecuteAction::CreateSP(this, &FHeartGraphEditor::JumpToNodeObjectDefinition),
			FCanExecuteAction::CreateSP(this, &FHeartGraphEditor::CanJumpToNodeObjectDefinition));
	}

	void FHeartGraphEditor::OnDetailsPanelCreated(const TSharedRef<SDetailsPanel, ESPMode::ThreadSafe>& DetailsView)
	{
		DetailsPanel = DetailsView;
	}

	void FHeartGraphEditor::OnNodePaletteCreated(const TSharedRef<SHeartPalette, ESPMode::ThreadSafe>& NodePalette)
	{
		Palette = NodePalette;
	}

	TSharedRef<SGraphEditor> FHeartGraphEditor::CreateGraphWidget(const FWorkflowTabSpawnInfo& Info)
	{
		SGraphEditor::FGraphEditorEvents InEvents;
		InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FHeartGraphEditor::OnSelectedNodesChanged);
		InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FHeartGraphEditor::OnNodeDoubleClicked);
		InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FHeartGraphEditor::OnNodeTitleCommitted);
		InEvents.OnSpawnNodeByShortcut = SGraphEditor::FOnSpawnNodeByShortcut::CreateStatic(&FHeartGraphEditor::OnSpawnGraphNodeByShortcut, HeartGraph->GetEdGraph());

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

	void FHeartGraphEditor::UndoGraphAction()
	{
		GEditor->UndoTransaction();
	}

	void FHeartGraphEditor::RedoGraphAction()
	{
		GEditor->RedoTransaction();
	}

	FReply FHeartGraphEditor::OnSpawnGraphNodeByShortcut(FInputChord InChord, const FVector2D& InPosition, UEdGraph* InGraph)
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

	void FHeartGraphEditor::SetUISelectionState(const FName SelectionOwner)
	{
		if (SelectionOwner != CurrentUISelection)
		{
			ClearSelectionStateFor(CurrentUISelection);
			CurrentUISelection = SelectionOwner;
		}
	}

	void FHeartGraphEditor::ClearSelectionStateFor(const FName SelectionOwner)
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

	void FHeartGraphEditor::OnCreateComment() const
	{
		FHeartGraphSchemaAction_NewComment CommentAction;
		CommentAction.PerformAction(HeartGraph->GetEdGraph(), nullptr, GraphEditor->GetPasteLocation());
	}

	void FHeartGraphEditor::OnStraightenConnections() const
	{
		GraphEditor->OnStraightenConnections();
	}

	bool FHeartGraphEditor::CanEdit() const
	{
		return GEditor->PlayWorld == nullptr;
	}

	EVisibility FHeartGraphEditor::GetDebuggerVisibility()
	{
		return GEditor->PlayWorld ? EVisibility::Visible : EVisibility::Collapsed;
	}

	TSet<UHeartEdGraphNode*> FHeartGraphEditor::GetSelectedHeartGraphNodes() const
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

	int32 FHeartGraphEditor::GetNumberOfSelectedNodes() const
	{
		return GraphEditor->GetSelectedNodes().Num();
	}

	bool FHeartGraphEditor::GetBoundsForSelectedNodes(FSlateRect& Rect, const float Padding) const
	{
		return GraphEditor->GetBoundsForSelectedNodes(Rect, Padding);
	}

	void FHeartGraphEditor::OnSelectedNodesChanged(const TSet<UObject*>& Nodes)
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

	void FHeartGraphEditor::SelectSingleNode(UEdGraphNode* Node) const
	{
		if (GraphEditor)
		{
			GraphEditor->ClearSelectionSet();
	        GraphEditor->SetNodeSelection(Node, true);
		}

	}

	void FHeartGraphEditor::SelectAllNodes() const
	{
		if (GraphEditor)
		{
			GraphEditor->SelectAllNodes();
		}
	}

	bool FHeartGraphEditor::CanSelectAllNodes() const
	{
		return true;
	}

	void FHeartGraphEditor::DeleteNode(UEdGraphNode* Node)
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

	void FHeartGraphEditor::DeleteSelectedNodes()
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

	void FHeartGraphEditor::DeleteSelectedDuplicableNodes()
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

	bool FHeartGraphEditor::CanDeleteNodes() const
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

	void FHeartGraphEditor::CutSelectedNodes()
	{
		CopySelectedNodes();

		// Cut should only delete nodes that can be duplicated
		DeleteSelectedDuplicableNodes();
	}

	bool FHeartGraphEditor::CanCutNodes() const
	{
		return CanCopyNodes() && CanDeleteNodes();
	}

	void FHeartGraphEditor::CopySelectedNodes() const
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

	bool FHeartGraphEditor::CanCopyNodes() const
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

	void FHeartGraphEditor::PasteNodes()
	{
		PasteNodesHere(GraphEditor->GetPasteLocation());
	}

	void FHeartGraphEditor::PasteNodesHere(const FVector2D& Location)
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

	bool FHeartGraphEditor::CanPasteNodes() const
	{
		if (CanEdit())
		{
			FString ClipboardContent;
			FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

			return FEdGraphUtilities::CanImportNodesFromText(HeartGraph->GetEdGraph(), ClipboardContent);
		}

		return false;
	}

	void FHeartGraphEditor::DuplicateNodes()
	{
		CopySelectedNodes();
		PasteNodes();
	}

	bool FHeartGraphEditor::CanDuplicateNodes() const
	{
		return CanCopyNodes();
	}

	void FHeartGraphEditor::OnNodeDoubleClicked(UEdGraphNode* Node) const
	{
		if (auto&& HeartGraphNode = Cast<UHeartEdGraphNode>(Node)->GetHeartGraphNode())
		{
			Node->JumpToDefinition();
		}
	}

	void FHeartGraphEditor::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged)
	{
		if (NodeBeingChanged)
		{
			const FScopedTransaction Transaction(LOCTEXT("RenameNode", "Rename Node"));
			NodeBeingChanged->Modify();
			NodeBeingChanged->OnRenameNode(NewText.ToString());
		}
	}

	void FHeartGraphEditor::AddInput() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->AddUserInput();
		}
	}

	bool FHeartGraphEditor::CanAddInput() const
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

	void FHeartGraphEditor::AddOutput() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->AddUserOutput();
		}
	}

	bool FHeartGraphEditor::CanAddOutput() const
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

	void FHeartGraphEditor::RemovePin() const
	{
		if (auto&& SelectedPin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& SelectedNode = Cast<UHeartEdGraphNode>(SelectedPin->GetOwningNode()))
			{
				SelectedNode->RemoveInstancePin(SelectedPin);
			}
		}
	}

	bool FHeartGraphEditor::CanRemovePin() const
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

	void FHeartGraphEditor::OnAddBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->NodeBreakpoint.AddBreakpoint();
		}
	}

	void FHeartGraphEditor::OnAddPinBreakpoint() const
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

	bool FHeartGraphEditor::CanAddBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			return !SelectedNode->NodeBreakpoint.HasBreakpoint();
		}

		return false;
	}

	bool FHeartGraphEditor::CanAddPinBreakpoint() const
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

	void FHeartGraphEditor::OnRemoveBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->NodeBreakpoint.RemoveBreakpoint();
		}
	}

	void FHeartGraphEditor::OnRemovePinBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				GraphNode->PinBreakpoints.Remove(Pin);
			}
		}
	}

	bool FHeartGraphEditor::CanRemoveBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			return SelectedNode->NodeBreakpoint.HasBreakpoint();
		}

		return false;
	}

	bool FHeartGraphEditor::CanRemovePinBreakpoint() const
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

	void FHeartGraphEditor::OnEnableBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->NodeBreakpoint.EnableBreakpoint();
		}
	}

	void FHeartGraphEditor::OnEnablePinBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				GraphNode->PinBreakpoints[Pin].EnableBreakpoint();
			}
		}
	}

	bool FHeartGraphEditor::CanEnableBreakpoint() const
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

	bool FHeartGraphEditor::CanEnablePinBreakpoint() const
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

	void FHeartGraphEditor::OnDisableBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->NodeBreakpoint.DisableBreakpoint();
		}
	}

	void FHeartGraphEditor::OnDisablePinBreakpoint() const
	{
		if (auto&& Pin = GraphEditor->GetGraphPinForMenu())
		{
			if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
			{
				GraphNode->PinBreakpoints[Pin].DisableBreakpoint();
			}
		}
	}

	bool FHeartGraphEditor::CanDisableBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			return SelectedNode->NodeBreakpoint.IsBreakpointEnabled();
		}

		return false;
	}

	bool FHeartGraphEditor::CanDisablePinBreakpoint() const
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

	void FHeartGraphEditor::OnToggleBreakpoint() const
	{
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->NodeBreakpoint.ToggleBreakpoint();
		}
	}

	void FHeartGraphEditor::OnTogglePinBreakpoint() const
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

	bool FHeartGraphEditor::CanToggleBreakpoint() const
	{
		return GetSelectedHeartGraphNodes().Num() > 0;
	}

	bool FHeartGraphEditor::CanTogglePinBreakpoint() const
	{
		return GraphEditor->GetGraphPinForMenu() != nullptr;
	}

	void FHeartGraphEditor::JumpToGraphNodeDefinition() const
	{
		// Iterator used but should only contain one node
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->JumpToDefinition();
			return;
		}
	}

	bool FHeartGraphEditor::CanJumpToGraphNodeDefinition() const
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

	void FHeartGraphEditor::JumpToNodeObjectDefinition() const
	{
		// Iterator used but should only contain one node
		for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
		{
			SelectedNode->JumpToNodeDefinition();
			return;
		}
	}

	bool FHeartGraphEditor::CanJumpToNodeObjectDefinition() const
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
