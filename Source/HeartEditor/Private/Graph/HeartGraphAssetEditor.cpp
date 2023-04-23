// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartGraphAssetEditor.h"

#include "Graph/HeartGraphAssetToolbar.h"
#include "HeartEditorCommands.h"
#include "Graph/HeartEdGraph.h"
#include "Graph/HeartEdGraphSchema.h"
#include "Graph/HeartEdGraphSchema_Actions.h"
#include "Nodes/HeartEdGraphNode.h"
#include "Graph/Widgets/SHeartPalette.h"

#include "Model/HeartGraph.h"
#include "Model/HeartGraphNode.h"

#include "EdGraphUtilities.h"
#include "EdGraph/EdGraphNode.h"
#include "Editor.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditor.h"
#include "GraphEditorActions.h"
#include "HeartEditorModule.h"
#include "HAL/PlatformApplicationMisc.h"
#include "IDetailsView.h"
#include "Kismet2/DebuggerCommands.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "ScopedTransaction.h"
#include "SNodePanel.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "HeartAssetEditor"

const FName FHeartGraphAssetEditor::DetailsTab("Details");
const FName FHeartGraphAssetEditor::GraphTab("Graph");
const FName FHeartGraphAssetEditor::PaletteTab("Palette");

FHeartGraphAssetEditor::FHeartGraphAssetEditor()
	: HeartGraph(nullptr)
{
}

FHeartGraphAssetEditor::~FHeartGraphAssetEditor()
{
	GEditor->UnregisterForUndo(this);
}

void FHeartGraphAssetEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(HeartGraph);
}

void FHeartGraphAssetEditor::PostUndo(bool bSuccess)
{
	HandleUndoTransaction();
}

void FHeartGraphAssetEditor::PostRedo(bool bSuccess)
{
	HandleUndoTransaction();
}

void FHeartGraphAssetEditor::HandleUndoTransaction()
{
	SetUISelectionState(NAME_None);
	FocusedGraphEditor->NotifyGraphChanged();
	FSlateApplication::Get().DismissAllMenus();
}

void FHeartGraphAssetEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		FocusedGraphEditor->NotifyGraphChanged();
	}
}

FName FHeartGraphAssetEditor::GetToolkitFName() const
{
	return FName("HeartEditor");
}

FText FHeartGraphAssetEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "HeartGraph Editor");
}

FString FHeartGraphAssetEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "HeartGraph").ToString();
}

FLinearColor FHeartGraphAssetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

void FHeartGraphAssetEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_HeartGraphAssetEditor", "HeartGraph Editor"));
	const auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(GraphTab, FOnSpawnTab::CreateSP(this, &FHeartGraphAssetEditor::SpawnTab_GraphCanvas))
				.SetDisplayName(LOCTEXT("GraphTab", "Viewport"))
				.SetGroup(WorkspaceMenuCategoryRef)
				.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(DetailsTab, FOnSpawnTab::CreateSP(this, &FHeartGraphAssetEditor::SpawnTab_Details))
				.SetDisplayName(LOCTEXT("DetailsTab", "Details"))
				.SetGroup(WorkspaceMenuCategoryRef)
				.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(PaletteTab, FOnSpawnTab::CreateSP(this, &FHeartGraphAssetEditor::SpawnTab_Palette))
				.SetDisplayName(LOCTEXT("PaletteTab", "Palette"))
				.SetGroup(WorkspaceMenuCategoryRef)
				.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Kismet.Tabs.Palette"));
}

void FHeartGraphAssetEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(GraphTab);
	InTabManager->UnregisterTabSpawner(DetailsTab);
	InTabManager->UnregisterTabSpawner(PaletteTab);
}

TSharedRef<SDockTab> FHeartGraphAssetEditor::SpawnTab_Details(const FSpawnTabArgs& Args) const
{
	check(Args.GetTabId() == DetailsTab);

	return SNew(SDockTab)
		.Label(LOCTEXT("HeartDetailsTitle", "Details"))
		[
			SNew(SSplitter)
			.Orientation(Orient_Vertical)
				+ SSplitter::Slot()
				[
					DetailsView_Graph.ToSharedRef()
				]
				+ SSplitter::Slot()
				[
					DetailsView_Object.ToSharedRef()
				]
		];
}

TSharedRef<SDockTab> FHeartGraphAssetEditor::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args) const
{
	check(Args.GetTabId() == GraphTab);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("HeartGraphTitle", "Graph"));

	if (FocusedGraphEditor.IsValid())
	{
		SpawnedTab->SetContent(FocusedGraphEditor.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FHeartGraphAssetEditor::SpawnTab_Palette(const FSpawnTabArgs& Args) const
{
	check(Args.GetTabId() == PaletteTab);

	return SNew(SDockTab)
		.Label(LOCTEXT("HeartPaletteTitle", "Palette"))
		[
			Palette.ToSharedRef()
		];
}

void FHeartGraphAssetEditor::InitHeartGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* ObjectToEdit)
{
	HeartGraph = CastChecked<UHeartGraph>(ObjectToEdit);

	// Support undo/redo
	HeartGraph->SetFlags(RF_Transactional);
	GEditor->RegisterForUndo(this);

	BindToolbarCommands();
	CreateToolbar();

	BindGraphCommands();
	CreateWidgets();

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("HeartAssetEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.225f)
					->AddTab(DetailsTab, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.65f)
					->AddTab(GraphTab, ETabState::OpenedTab)->SetHideTabWell(true)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.125f)
					->AddTab(PaletteTab, ETabState::OpenedTab)
				)
		);

	constexpr bool bCreateDefaultStandaloneMenu = true;
	constexpr bool bCreateDefaultToolbar = true;
	InitAssetEditor(Mode, InitToolkitHost, TEXT("HeartEditorApp"), StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectToEdit, false);

	RegenerateMenusAndToolbars();
}

void FHeartGraphAssetEditor::CreateToolbar()
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

void FHeartGraphAssetEditor::BindToolbarCommands()
{
	FHeartGraphToolbarCommands::Register();
	const FHeartGraphToolbarCommands& ToolbarCommands = FHeartGraphToolbarCommands::Get();

	// Editing
	ToolkitCommands->MapAction(ToolbarCommands.RefreshAsset,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::RefreshAsset),
		FCanExecuteAction::CreateStatic(&FHeartGraphAssetEditor::CanEdit));

	// Engine's Play commands
	// @todo we probably dont need these??
	ToolkitCommands->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());
}

void FHeartGraphAssetEditor::RefreshAsset()
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

void FHeartGraphAssetEditor::CreateWidgets()
{
	if (IsValid(HeartGraph->GetEdGraph()))
	{
		FocusedGraphEditor = CreateGraphWidget();
	}
	else
	{
		UE_LOG(LogHeartEditor, Error, TEXT("HeartEdGraph is invalid for HeartGraph '%s'!"), *HeartGraph->GetName())
	}

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.bShowPropertyMatrixButton = false;
	Args.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
	Args.NotifyHook = this;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	DetailsView_Graph = PropertyModule.CreateDetailView(Args);
	DetailsView_Graph->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateStatic(&FHeartGraphAssetEditor::CanEdit));
	DetailsView_Graph->SetObject(HeartGraph);

	DetailsView_Object = PropertyModule.CreateDetailView(Args);
	DetailsView_Object->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateStatic(&FHeartGraphAssetEditor::CanEdit));
	DetailsView_Object->SetObject(nullptr);

	Palette = SNew(SHeartPalette, SharedThis(this));
}

TSharedRef<SGraphEditor> FHeartGraphAssetEditor::CreateGraphWidget()
{
	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FHeartGraphAssetEditor::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FHeartGraphAssetEditor::OnNodeDoubleClicked);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FHeartGraphAssetEditor::OnNodeTitleCommitted);
	InEvents.OnSpawnNodeByShortcut = SGraphEditor::FOnSpawnNodeByShortcut::CreateStatic(&FHeartGraphAssetEditor::OnSpawnGraphNodeByShortcut, HeartGraph->GetEdGraph());

	return SNew(SGraphEditor)
		.AdditionalCommands(ToolkitCommands)
		.IsEditable(true)
		.Appearance(GetGraphAppearanceInfo())
		.GraphToEdit(HeartGraph->GetEdGraph())
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}

FGraphAppearanceInfo FHeartGraphAssetEditor::GetGraphAppearanceInfo() const
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = GetCornerText();
	return AppearanceInfo;
}

FText FHeartGraphAssetEditor::GetCornerText() const
{
	if (ensure(IsValid(HeartGraph)))
	{
		return FText::FromString(HeartGraph->GetGraphTypeName().ToString().ToUpper());
	}

	return LOCTEXT("AppearanceCornerText_HeartGraphErrorText", "UNKNOWN");
}

void FHeartGraphAssetEditor::BindGraphCommands()
{
	FGraphEditorCommands::Register();
	FHeartGraphCommands::Register();
	FHeartSpawnNodeCommands::Register();

	const FGenericCommands& GenericCommands = FGenericCommands::Get();
	const FGraphEditorCommandsImpl& GraphCommands = FGraphEditorCommands::Get();
	const FHeartGraphCommands& HeartGraphCommands = FHeartGraphCommands::Get();

	// Graph commands
	ToolkitCommands->MapAction(GraphCommands.CreateComment,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::OnCreateComment),
		FCanExecuteAction::CreateStatic(&FHeartGraphAssetEditor::CanEdit));

	ToolkitCommands->MapAction(GraphCommands.StraightenConnections,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::OnStraightenConnections));

	// Generic Node commands
	ToolkitCommands->MapAction(GenericCommands.Undo,
		FExecuteAction::CreateStatic(&FHeartGraphAssetEditor::UndoGraphAction),
		FCanExecuteAction::CreateStatic(&FHeartGraphAssetEditor::CanEdit));

	ToolkitCommands->MapAction(GenericCommands.Redo,
		FExecuteAction::CreateStatic(&FHeartGraphAssetEditor::RedoGraphAction),
		FCanExecuteAction::CreateStatic(&FHeartGraphAssetEditor::CanEdit));

	ToolkitCommands->MapAction(GenericCommands.SelectAll,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::SelectAllNodes),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanSelectAllNodes));

	ToolkitCommands->MapAction(GenericCommands.Delete,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanDeleteNodes));

	ToolkitCommands->MapAction(GenericCommands.Copy,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CopySelectedNodes),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanCopyNodes));

	ToolkitCommands->MapAction(GenericCommands.Cut,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CutSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanCutNodes));

	ToolkitCommands->MapAction(GenericCommands.Paste,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::PasteNodes),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanPasteNodes));

	ToolkitCommands->MapAction(GenericCommands.Duplicate,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::DuplicateNodes),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanDuplicateNodes));

	// Pin commands
	ToolkitCommands->MapAction(HeartGraphCommands.AddInput,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::AddInput),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanAddInput));

	ToolkitCommands->MapAction(HeartGraphCommands.AddOutput,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::AddOutput),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanAddOutput));

	ToolkitCommands->MapAction(HeartGraphCommands.RemovePin,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::RemovePin),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanRemovePin));

	// Breakpoint commands
	ToolkitCommands->MapAction(GraphCommands.AddBreakpoint,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::OnAddBreakpoint),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanAddBreakpoint),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateSP(this, &FHeartGraphAssetEditor::CanAddBreakpoint)
	);

	ToolkitCommands->MapAction(GraphCommands.RemoveBreakpoint,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::OnRemoveBreakpoint),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanRemoveBreakpoint),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateSP(this, &FHeartGraphAssetEditor::CanRemoveBreakpoint)
	);

	ToolkitCommands->MapAction(GraphCommands.EnableBreakpoint,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::OnEnableBreakpoint),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanEnableBreakpoint),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateSP(this, &FHeartGraphAssetEditor::CanEnableBreakpoint)
	);

	ToolkitCommands->MapAction(GraphCommands.DisableBreakpoint,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::OnDisableBreakpoint),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanDisableBreakpoint),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateSP(this, &FHeartGraphAssetEditor::CanDisableBreakpoint)
	);

	ToolkitCommands->MapAction(GraphCommands.ToggleBreakpoint,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::OnToggleBreakpoint),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanToggleBreakpoint),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateSP(this, &FHeartGraphAssetEditor::CanToggleBreakpoint)
	);

	// Pin Breakpoint commands
	ToolkitCommands->MapAction(HeartGraphCommands.AddPinBreakpoint,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::OnAddPinBreakpoint),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanAddPinBreakpoint),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateSP(this, &FHeartGraphAssetEditor::CanAddPinBreakpoint)
	);

	ToolkitCommands->MapAction(HeartGraphCommands.RemovePinBreakpoint,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::OnRemovePinBreakpoint),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanRemovePinBreakpoint),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateSP(this, &FHeartGraphAssetEditor::CanRemovePinBreakpoint)
	);

	ToolkitCommands->MapAction(HeartGraphCommands.EnablePinBreakpoint,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::OnEnablePinBreakpoint),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanEnablePinBreakpoint),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateSP(this, &FHeartGraphAssetEditor::CanEnablePinBreakpoint)
	);

	ToolkitCommands->MapAction(HeartGraphCommands.DisablePinBreakpoint,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::OnDisablePinBreakpoint),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanDisablePinBreakpoint),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateSP(this, &FHeartGraphAssetEditor::CanDisablePinBreakpoint)
	);

	ToolkitCommands->MapAction(HeartGraphCommands.TogglePinBreakpoint,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::OnTogglePinBreakpoint),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanTogglePinBreakpoint),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateSP(this, &FHeartGraphAssetEditor::CanTogglePinBreakpoint)
	);

	// Jump commands
	ToolkitCommands->MapAction(HeartGraphCommands.JumpToGraphNodeDefinition,
        FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::JumpToGraphNodeDefinition),
        FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanJumpToGraphNodeDefinition));

	// Jump commands
	ToolkitCommands->MapAction(HeartGraphCommands.JumpToNodeDefinition,
		FExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::JumpToNodeObjectDefinition),
		FCanExecuteAction::CreateSP(this, &FHeartGraphAssetEditor::CanJumpToNodeObjectDefinition));
}

void FHeartGraphAssetEditor::UndoGraphAction()
{
	GEditor->UndoTransaction();
}

void FHeartGraphAssetEditor::RedoGraphAction()
{
	GEditor->RedoTransaction();
}

FReply FHeartGraphAssetEditor::OnSpawnGraphNodeByShortcut(FInputChord InChord, const FVector2D& InPosition, UEdGraph* InGraph)
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

void FHeartGraphAssetEditor::SetUISelectionState(const FName SelectionOwner)
{
	if (SelectionOwner != CurrentUISelection)
	{
		ClearSelectionStateFor(CurrentUISelection);
		CurrentUISelection = SelectionOwner;
	}
}

void FHeartGraphAssetEditor::ClearSelectionStateFor(const FName SelectionOwner)
{
	if (SelectionOwner == GraphTab)
	{
		FocusedGraphEditor->ClearSelectionSet();
	}
	else if (SelectionOwner == PaletteTab)
	{
		if (Palette.IsValid())
		{
			Palette->ClearGraphActionMenuSelection();
		}
	}
}

void FHeartGraphAssetEditor::OnCreateComment() const
{
	FHeartGraphSchemaAction_NewComment CommentAction;
	CommentAction.PerformAction(HeartGraph->GetEdGraph(), nullptr, FocusedGraphEditor->GetPasteLocation());
}

void FHeartGraphAssetEditor::OnStraightenConnections() const
{
	FocusedGraphEditor->OnStraightenConnections();
}

bool FHeartGraphAssetEditor::CanEdit()
{
	return GEditor->PlayWorld == nullptr;
}

EVisibility FHeartGraphAssetEditor::GetDebuggerVisibility()
{
	return GEditor->PlayWorld ? EVisibility::Visible : EVisibility::Collapsed;
}

TSet<UHeartEdGraphNode*> FHeartGraphAssetEditor::GetSelectedHeartGraphNodes() const
{
	TSet<UHeartEdGraphNode*> Result;

	const FGraphPanelSelectionSet SelectedNodes = FocusedGraphEditor->GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		if (auto&& SelectedNode = Cast<UHeartEdGraphNode>(*NodeIt))
		{
			Result.Emplace(SelectedNode);
		}
	}

	return Result;
}

int32 FHeartGraphAssetEditor::GetNumberOfSelectedNodes() const
{
	return FocusedGraphEditor->GetSelectedNodes().Num();
}

bool FHeartGraphAssetEditor::GetBoundsForSelectedNodes(class FSlateRect& Rect, float Padding) const
{
	return FocusedGraphEditor->GetBoundsForSelectedNodes(Rect, Padding);
}

void FHeartGraphAssetEditor::OnSelectedNodesChanged(const TSet<UObject*>& Nodes)
{
	TArray<UObject*> SelectedGraphObjects;
	TArray<UObject*> SelectedObjects;

	if (Nodes.Num() > 0)
	{
		SetUISelectionState(GraphTab);

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

	if (DetailsView_Graph.IsValid())
	{
		DetailsView_Graph->SetObjects(SelectedGraphObjects);
	}

	if (DetailsView_Object.IsValid())
	{
		DetailsView_Object->SetObjects(SelectedObjects);
	}
}

void FHeartGraphAssetEditor::SelectSingleNode(UEdGraphNode* Node) const
{
	FocusedGraphEditor->ClearSelectionSet();
	FocusedGraphEditor->SetNodeSelection(Node, true);
}

void FHeartGraphAssetEditor::SelectAllNodes() const
{
	FocusedGraphEditor->SelectAllNodes();
}

bool FHeartGraphAssetEditor::CanSelectAllNodes() const
{
	return true;
}

void FHeartGraphAssetEditor::DeleteNode(UEdGraphNode* Node)
{
	check(Node);

	// Try to remove the Runtime Node first, if there is one.
	if (auto&& HeartEdGraphNode = Cast<UHeartEdGraphNode>(Node))
	{
		if (auto&& RuntimeNode = HeartEdGraphNode->GetHeartGraphNode())
		{
			HeartGraph->RemoveNode(RuntimeNode->GetGuid());
		}
	}

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

	Node->DestroyNode();
}

void FHeartGraphAssetEditor::DeleteSelectedNodes()
{
	const FScopedTransaction Transaction(LOCTEXT("DeleteSelectedNode", "Delete Selected Node"));
	FocusedGraphEditor->GetCurrentGraph()->Modify();
	HeartGraph->Modify();

	const FGraphPanelSelectionSet SelectedNodes = FocusedGraphEditor->GetSelectedNodes();
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

void FHeartGraphAssetEditor::DeleteSelectedDuplicableNodes()
{
	// Cache off the old selection
	const FGraphPanelSelectionSet OldSelectedNodes = FocusedGraphEditor->GetSelectedNodes();

	// Clear the selection and only select the nodes that can be duplicated
	FGraphPanelSelectionSet RemainingNodes;
	FocusedGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIt(OldSelectedNodes); SelectedIt; ++SelectedIt)
	{
		if (auto&& Node = Cast<UEdGraphNode>(*SelectedIt))
		{
			if (Node->CanDuplicateNode())
			{
				FocusedGraphEditor->SetNodeSelection(Node, true);
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
			FocusedGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

bool FHeartGraphAssetEditor::CanDeleteNodes() const
{
	if (CanEdit())
	{
		const FGraphPanelSelectionSet SelectedNodes = FocusedGraphEditor->GetSelectedNodes();
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

void FHeartGraphAssetEditor::CutSelectedNodes()
{
	CopySelectedNodes();

	// Cut should only delete nodes that can be duplicated
	DeleteSelectedDuplicableNodes();
}

bool FHeartGraphAssetEditor::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FHeartGraphAssetEditor::CopySelectedNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = FocusedGraphEditor->GetSelectedNodes();
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

bool FHeartGraphAssetEditor::CanCopyNodes() const
{
	if (CanEdit())
	{
		const FGraphPanelSelectionSet SelectedNodes = FocusedGraphEditor->GetSelectedNodes();
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

void FHeartGraphAssetEditor::PasteNodes()
{
	PasteNodesHere(FocusedGraphEditor->GetPasteLocation());
}

void FHeartGraphAssetEditor::PasteNodesHere(const FVector2D& Location)
{
	SetUISelectionState(NAME_None);

	// Undo/Redo support
	const FScopedTransaction Transaction(LOCTEXT("PasteNode", "Paste Node"));
	HeartGraph->GetEdGraph()->Modify();
	HeartGraph->Modify();

	// Clear the selection set (newly pasted stuff will be selected)
	FocusedGraphEditor->ClearSelectionSet();

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
		FocusedGraphEditor->SetNodeSelection(Node, true);

		Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
		Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

		Node->SnapToGrid(SNodePanel::GetSnapGridSize());
	}

	// @todo probably broadcast something here instead, or handle in child implementation
	//HeartGraph->HarvestNodeConnections();

	// Update UI
	FocusedGraphEditor->NotifyGraphChanged();

	HeartGraph->PostEditChange();
	HeartGraph->MarkPackageDirty();
}

bool FHeartGraphAssetEditor::CanPasteNodes() const
{
	if (CanEdit())
	{
		FString ClipboardContent;
		FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

		return FEdGraphUtilities::CanImportNodesFromText(HeartGraph->GetEdGraph(), ClipboardContent);
	}

	return false;
}

void FHeartGraphAssetEditor::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FHeartGraphAssetEditor::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FHeartGraphAssetEditor::OnNodeDoubleClicked(UEdGraphNode* Node) const
{
	if (auto&& HeartGraphNode = Cast<UHeartEdGraphNode>(Node)->GetHeartGraphNode())
	{
		Node->JumpToDefinition();
	}
}

void FHeartGraphAssetEditor::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged)
{
	if (NodeBeingChanged)
	{
		const FScopedTransaction Transaction(LOCTEXT("RenameNode", "Rename Node"));
		NodeBeingChanged->Modify();
		NodeBeingChanged->OnRenameNode(NewText.ToString());
	}
}

void FHeartGraphAssetEditor::AddInput() const
{
	for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
	{
		SelectedNode->AddUserInput();
	}
}

bool FHeartGraphAssetEditor::CanAddInput() const
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

void FHeartGraphAssetEditor::AddOutput() const
{
	for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
	{
		SelectedNode->AddUserOutput();
	}
}

bool FHeartGraphAssetEditor::CanAddOutput() const
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

void FHeartGraphAssetEditor::RemovePin() const
{
	if (auto&& SelectedPin = FocusedGraphEditor->GetGraphPinForMenu())
	{
		if (auto&& SelectedNode = Cast<UHeartEdGraphNode>(SelectedPin->GetOwningNode()))
		{
			SelectedNode->RemoveInstancePin(SelectedPin);
		}
	}
}

bool FHeartGraphAssetEditor::CanRemovePin() const
{
	if (CanEdit() && GetSelectedHeartGraphNodes().Num() == 1)
	{
		if (auto&& Pin = FocusedGraphEditor->GetGraphPinForMenu())
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

void FHeartGraphAssetEditor::OnAddBreakpoint() const
{
	for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
	{
		SelectedNode->NodeBreakpoint.AddBreakpoint();
	}
}

void FHeartGraphAssetEditor::OnAddPinBreakpoint() const
{
	if (auto&& Pin = FocusedGraphEditor->GetGraphPinForMenu())
	{
		if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
		{
			GraphNode->PinBreakpoints.Add(Pin, FHeartBreakpoint());
			GraphNode->PinBreakpoints[Pin].AddBreakpoint();
		}
	}
}

bool FHeartGraphAssetEditor::CanAddBreakpoint() const
{
	for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
	{
		return !SelectedNode->NodeBreakpoint.HasBreakpoint();
	}

	return false;
}

bool FHeartGraphAssetEditor::CanAddPinBreakpoint() const
{
	if (auto&& Pin = FocusedGraphEditor->GetGraphPinForMenu())
	{
		if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
		{
			return !GraphNode->PinBreakpoints.Contains(Pin) || !GraphNode->PinBreakpoints[Pin].HasBreakpoint();
		}
	}

	return false;
}

void FHeartGraphAssetEditor::OnRemoveBreakpoint() const
{
	for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
	{
		SelectedNode->NodeBreakpoint.RemoveBreakpoint();
	}
}

void FHeartGraphAssetEditor::OnRemovePinBreakpoint() const
{
	if (auto&& Pin = FocusedGraphEditor->GetGraphPinForMenu())
	{
		if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
		{
			GraphNode->PinBreakpoints.Remove(Pin);
		}
	}
}

bool FHeartGraphAssetEditor::CanRemoveBreakpoint() const
{
	for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
	{
		return SelectedNode->NodeBreakpoint.HasBreakpoint();
	}

	return false;
}

bool FHeartGraphAssetEditor::CanRemovePinBreakpoint() const
{
	if (auto&& Pin = FocusedGraphEditor->GetGraphPinForMenu())
	{
		if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
		{
			return GraphNode->PinBreakpoints.Contains(Pin);
		}
	}

	return false;
}

void FHeartGraphAssetEditor::OnEnableBreakpoint() const
{
	for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
	{
		SelectedNode->NodeBreakpoint.EnableBreakpoint();
	}
}

void FHeartGraphAssetEditor::OnEnablePinBreakpoint() const
{
	if (auto&& Pin = FocusedGraphEditor->GetGraphPinForMenu())
	{
		if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
		{
			GraphNode->PinBreakpoints[Pin].EnableBreakpoint();
		}
	}
}

bool FHeartGraphAssetEditor::CanEnableBreakpoint() const
{
	if (auto&& Pin = FocusedGraphEditor->GetGraphPinForMenu())
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

bool FHeartGraphAssetEditor::CanEnablePinBreakpoint() const
{
	if (auto&& Pin = FocusedGraphEditor->GetGraphPinForMenu())
	{
		if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
		{
			return GraphNode->PinBreakpoints.Contains(Pin) && GraphNode->PinBreakpoints[Pin].CanEnableBreakpoint();
		}
	}

	return false;
}

void FHeartGraphAssetEditor::OnDisableBreakpoint() const
{
	for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
	{
		SelectedNode->NodeBreakpoint.DisableBreakpoint();
	}
}

void FHeartGraphAssetEditor::OnDisablePinBreakpoint() const
{
	if (auto&& Pin = FocusedGraphEditor->GetGraphPinForMenu())
	{
		if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
		{
			GraphNode->PinBreakpoints[Pin].DisableBreakpoint();
		}
	}
}

bool FHeartGraphAssetEditor::CanDisableBreakpoint() const
{
	for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
	{
		return SelectedNode->NodeBreakpoint.IsBreakpointEnabled();
	}

	return false;
}

bool FHeartGraphAssetEditor::CanDisablePinBreakpoint() const
{
	if (auto&& Pin = FocusedGraphEditor->GetGraphPinForMenu())
	{
		if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
		{
			return GraphNode->PinBreakpoints.Contains(Pin) && GraphNode->PinBreakpoints[Pin].IsBreakpointEnabled();
		}
	}

	return false;
}

void FHeartGraphAssetEditor::OnToggleBreakpoint() const
{
	for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
	{
		SelectedNode->NodeBreakpoint.ToggleBreakpoint();
	}
}

void FHeartGraphAssetEditor::OnTogglePinBreakpoint() const
{
	if (auto&& Pin = FocusedGraphEditor->GetGraphPinForMenu())
	{
		if (auto&& GraphNode = Cast<UHeartEdGraphNode>(Pin->GetOwningNode()))
		{
			GraphNode->PinBreakpoints.Add(Pin, FHeartBreakpoint());
			GraphNode->PinBreakpoints[Pin].ToggleBreakpoint();
		}
	}
}

bool FHeartGraphAssetEditor::CanToggleBreakpoint() const
{
	return GetSelectedHeartGraphNodes().Num() > 0;
}

bool FHeartGraphAssetEditor::CanTogglePinBreakpoint() const
{
	return FocusedGraphEditor->GetGraphPinForMenu() != nullptr;
}

void FHeartGraphAssetEditor::JumpToGraphNodeDefinition() const
{
	// Iterator used but should only contain one node
	for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
	{
		SelectedNode->JumpToDefinition();
		return;
	}
}

bool FHeartGraphAssetEditor::CanJumpToGraphNodeDefinition() const
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

void FHeartGraphAssetEditor::JumpToNodeObjectDefinition() const
{
	// Iterator used but should only contain one node
	for (auto&& SelectedNode : GetSelectedHeartGraphNodes())
	{
		SelectedNode->JumpToNodeDefinition();
		return;
	}
}

bool FHeartGraphAssetEditor::CanJumpToNodeObjectDefinition() const
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

#undef LOCTEXT_NAMESPACE
