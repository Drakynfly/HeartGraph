// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartGraphAssetToolbar.h"

#include "Graph/HeartGraphAssetEditor.h"
//#include "Graph/Widgets/SAssetRevisionMenu.h"
#include "HeartEditorCommands.h"

#include "Model/HeartGraph.h"

#include "Misc/Attribute.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "Widgets/SBoxPanel.h"

#include "AssetToolsModule.h"
#include "HeartEditorModule.h"
#include "IAssetTypeActions.h"
#include "IDocumentation.h"
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"
#include "SourceControlHelpers.h"
#include "Misc/MessageDialog.h"
#include "WorkflowOrientedApp/SModeWidget.h"

#define LOCTEXT_NAMESPACE "HeartDebuggerToolbar"


// Heart Graph Asset Breadcrumb

void SHeartGraphAssetBreadcrumb::Construct(const FArguments& InArgs, const TWeakObjectPtr<UHeartGraph> InTemplateAsset)
{
	TemplateAsset = InTemplateAsset;

	// create breadcrumb
	SAssignNew(BreadcrumbTrail, SBreadcrumbTrail<FHeartBreadcrumb>)
		.OnCrumbClicked(this, &SHeartGraphAssetBreadcrumb::OnCrumbClicked)
		.Visibility_Static(&Heart::AssetEditor::FHeartGraphEditor::GetDebuggerVisibility)
		.ButtonStyle(FAppStyle::Get(), "FlatButton")
		.DelimiterImage(FAppStyle::GetBrush("Sequencer.BreadcrumbIcon"))
		.PersistentBreadcrumbs(true)
		.TextStyle(FAppStyle::Get(), "Sequencer.BreadcrumbText");

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		.AutoHeight()
		.Padding(25.0f, 10.0f)
		[
			BreadcrumbTrail.ToSharedRef()
		]
	];

	// fill breadcrumb
	BreadcrumbTrail->ClearCrumbs();
	if (auto&& InspectedInstance = TemplateAsset.Get())
	{
		TArray<UHeartGraph*> InstancesFromRoot = {InspectedInstance};

		for (auto&& Instance : InstancesFromRoot)
		{
			TAttribute<FText> CrumbNameAttribute = MakeAttributeSP(this, &SHeartGraphAssetBreadcrumb::GetBreadcrumbText, Instance);
			BreadcrumbTrail->PushCrumb(CrumbNameAttribute, FHeartBreadcrumb(Instance));
		}
	}
}

void SHeartGraphAssetBreadcrumb::OnCrumbClicked(const FHeartBreadcrumb& Item) const
{
	ensure(TemplateAsset.IsValid());

	if (Item.InstanceName != TemplateAsset->GetFName()) // @todo GetFName could be replaced by GetDisplayName, etc
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Item.AssetPathName);
	}
}

FText SHeartGraphAssetBreadcrumb::GetBreadcrumbText(const TWeakObjectPtr<UHeartGraph> HeartGraph) const
{
	return HeartGraph.IsValid() ? FText::FromName(HeartGraph->GetFName()) : FText::GetEmpty(); // @todo GetFName could be replaced by GetDisplayName, etc
}


// Heart Graph Asset Toolbar

FHeartGraphAssetToolbar::FHeartGraphAssetToolbar(const TSharedPtr<Heart::AssetEditor::FHeartGraphEditor> InAssetEditor, UToolMenu* ToolbarMenu)
	: AssetEditor(InAssetEditor)
{
	BuildAssetToolbar(ToolbarMenu);
	BuildDebuggerToolbar(ToolbarMenu);
}

void FHeartGraphAssetToolbar::AddEditorModesToolbar(TSharedPtr<FExtender> Extender)
{
	const TSharedPtr<Heart::AssetEditor::FHeartGraphEditor> AssetEditorPtr = AssetEditor.Pin();

	Extender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		AssetEditorPtr->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FHeartGraphAssetToolbar::FillEditorModesToolbar));
}

void FHeartGraphAssetToolbar::FillEditorModesToolbar(FToolBarBuilder& ToolBarBuilder)
{
	const TSharedPtr<Heart::AssetEditor::FHeartGraphEditor> AssetEditorPtr = AssetEditor.Pin();
	const UHeartGraph* HeartGraph = AssetEditorPtr->GetHeartGraph();

	if (IsValid(HeartGraph))
	{
		const TAttribute<FName> GetActiveMode(AssetEditorPtr.ToSharedRef(), &Heart::AssetEditor::FHeartGraphEditor::GetCurrentMode);
		const FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(AssetEditorPtr.ToSharedRef(), &Heart::AssetEditor::FHeartGraphEditor::SetCurrentMode);

		// Left side padding
		AssetEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

		struct FTempRegisteredApplicationMode
		{
			FName ModeID;
			FText LocalizedMode;
			FText TooltipText;
		};

		const FHeartEditorModule& HeartEditorModule = FModuleManager::LoadModuleChecked<FHeartEditorModule>("HeartEditor");
		for (auto&& Mode : HeartEditorModule.GetApplicationModes())
		{
			const Heart::AssetEditor::FRegisteredApplicationMode& ModeData = Mode.Value;

			if (!ModeData.SupportsAsset.Execute(HeartGraph))
			{
				continue;
			}

			AssetEditorPtr->AddToolbarWidget(
				SNew(SModeWidget, ModeData.LocalizedName, Mode.Key)
				.OnGetActiveMode(GetActiveMode)
				.OnSetActiveMode(SetActiveMode)
				.CanBeSelected(AssetEditorPtr.Get(), &Heart::AssetEditor::FHeartGraphEditor::CanActivateMode, Mode.Key)
				.ToolTip(IDocumentation::Get()->CreateToolTip(
					ModeData.TooltipText,
					nullptr,
					TEXT("Editors/HeartEditor"),
					Mode.Key.ToString()))
				.IconImage(FAppStyle::GetBrush("FullBlueprintEditor.SwitchToScriptingMode"))
				.AddMetaData<FTagMetaData>(FTagMetaData(Mode.Key))
			);

			// Right side padding
			AssetEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(10.0f, 1.0f)));
		}
	}
}

void FHeartGraphAssetToolbar::BuildAssetToolbar(UToolMenu* ToolbarMenu) const
{
	FToolMenuSection& Section = ToolbarMenu->AddSection("Editing");
	Section.InsertPosition = FToolMenuInsert("Asset", EToolMenuInsertType::After);

	Section.AddEntry(FToolMenuEntry::InitToolBarButton(FHeartGraphToolbarCommands::Get().RefreshAsset));

	// visual diff: menu to choose asset revision compared with the current one
	FToolMenuSection& DiffSection = ToolbarMenu->AddSection("SourceControl");
	DiffSection.InsertPosition = FToolMenuInsert("Asset", EToolMenuInsertType::After);
	DiffSection.AddDynamicEntry("SourceControlCommands", FNewToolMenuSectionDelegate::CreateLambda([this](FToolMenuSection& InSection)
	{
		InSection.InsertPosition = FToolMenuInsert();
		FToolMenuEntry DiffEntry = FToolMenuEntry::InitComboButton(
			"Diff",
			FUIAction(),
			FOnGetContent::CreateRaw(this, &FHeartGraphAssetToolbar::MakeDiffMenu),
			LOCTEXT("Diff", "Diff"),
			LOCTEXT("HeartGraphAssetEditorDiffToolTip", "Diff against previous revisions"),
			FSlateIcon(FAppStyle::Get().GetStyleSetName(), "BlueprintDiff.ToolbarIcon")
		);
		DiffEntry.StyleNameOverride = "CalloutToolbar";
		InSection.AddEntry(DiffEntry);
	}));
}

/** Delegate called to diff a specific revision with the current */
// Copy from FBlueprintEditorToolbar::OnDiffRevisionPicked
static void OnDiffRevisionPicked(FRevisionInfo const& RevisionInfo, const FString& Filename, TWeakObjectPtr<UObject> CurrentAsset)
{
	ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();

	// Get the SCC state
	const FSourceControlStatePtr SourceControlState = SourceControlProvider.GetState(Filename, EStateCacheUsage::Use);
	if (SourceControlState.IsValid())
	{
		for (int32 HistoryIndex = 0; HistoryIndex < SourceControlState->GetHistorySize(); HistoryIndex++)
		{
			TSharedPtr<ISourceControlRevision, ESPMode::ThreadSafe> Revision = SourceControlState->GetHistoryItem(HistoryIndex);
			check(Revision.IsValid());
			if (Revision->GetRevision() == RevisionInfo.Revision)
			{
				// Get the revision of this package from source control
				FString PreviousTempPkgName;
				if (Revision->Get(PreviousTempPkgName))
				{
					// Try and load that package
					UPackage* PreviousTempPkg = LoadPackage(nullptr, *PreviousTempPkgName, LOAD_ForDiff | LOAD_DisableCompileOnLoad);
					if (PreviousTempPkg)
					{
						const FString PreviousAssetName = FPaths::GetBaseFilename(Filename, true);
						UObject* PreviousAsset = FindObject<UObject>(PreviousTempPkg, *PreviousAssetName);
						if (PreviousAsset)
						{
							const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
							const FRevisionInfo OldRevision = {Revision->GetRevision(), Revision->GetCheckInIdentifier(), Revision->GetDate()};
							const FRevisionInfo CurrentRevision = {TEXT(""), Revision->GetCheckInIdentifier(), Revision->GetDate()};
							AssetToolsModule.Get().DiffAssets(PreviousAsset, CurrentAsset.Get(), OldRevision, CurrentRevision);
						}
					}
					else
					{
						FMessageDialog::Open(EAppMsgType::Ok, NSLOCTEXT("SourceControl.HistoryWindow", "UnableToLoadAssets", "Unable to load assets to diff. Content may no longer be supported?"));
					}
				}
				break;
			}
		}
	}
}

// Variant of FBlueprintEditorToolbar::MakeDiffMenu
TSharedRef<SWidget> FHeartGraphAssetToolbar::MakeDiffMenu() const
{
	if (ISourceControlModule::Get().IsEnabled() && ISourceControlModule::Get().GetProvider().IsAvailable())
	{
		if (auto&& HeartGraph = AssetEditor.Pin()->GetHeartGraph())
		{
			FString Filename = SourceControlHelpers::PackageFilename(HeartGraph->GetPathName());
			TWeakObjectPtr<UObject> AssetPtr = HeartGraph;

			return SNullWidget::NullWidget;
			// Add our async SCC task widget
			//return SNew(SAssetRevisionMenu, Filename)
			//	.OnRevisionSelected_Static(&OnDiffRevisionPicked, AssetPtr);
		}
		else
		{
			// if asset is null then this means that multiple assets are selected
			FMenuBuilder MenuBuilder(true, nullptr);
			MenuBuilder.AddMenuEntry(LOCTEXT("NoRevisionsForMultipleHeartGraphAssets", "Multiple Heart Graph Assets selected"), FText::GetEmpty(), FSlateIcon(), FUIAction());
			return MenuBuilder.MakeWidget();
		}
	}

	FMenuBuilder MenuBuilder(true, nullptr);
	MenuBuilder.AddMenuEntry(LOCTEXT("SourceControlDisabled", "Source control is disabled"), FText::GetEmpty(), FSlateIcon(), FUIAction());
	return MenuBuilder.MakeWidget();
}

void FHeartGraphAssetToolbar::BuildDebuggerToolbar(UToolMenu* ToolbarMenu)
{
	FToolMenuSection& Section = ToolbarMenu->AddSection("Debugging");
	Section.InsertPosition = FToolMenuInsert("Asset", EToolMenuInsertType::After);

	auto&& TemplateAsset = AssetEditor.Pin()->GetHeartGraph();

	Breadcrumb = SNew(SHeartGraphAssetBreadcrumb, TemplateAsset);
	Section.AddEntry(FToolMenuEntry::InitWidget("AssetBreadcrumb", Breadcrumb.ToSharedRef(), FText::GetEmpty(), true));
}

#undef LOCTEXT_NAMESPACE