// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/HeartGraphAssetToolbar.h"

#include "Graph/HeartGraphAssetEditor.h"
//#include "Asset/SAssetRevisionMenu.h"
#include "HeartEditorCommands.h"

#include "Model/HeartGraph.h"

#include "Kismet2/DebuggerCommands.h"
#include "Misc/Attribute.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "Widgets/SBoxPanel.h"

#include "AssetToolsModule.h"
#include "IAssetTypeActions.h"
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"
#include "SourceControlHelpers.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "HeartDebuggerToolbar"

//////////////////////////////////////////////////////////////////////////
// Heart Graph Asset Breadcrumb

void SHeartGraphAssetBreadcrumb::Construct(const FArguments& InArgs, const TWeakObjectPtr<UHeartGraph> InTemplateAsset)
{
	TemplateAsset = InTemplateAsset;

	// create breadcrumb
	SAssignNew(BreadcrumbTrail, SBreadcrumbTrail<FHeartBreadcrumb>)
		.OnCrumbClicked(this, &SHeartGraphAssetBreadcrumb::OnCrumbClicked)
		.Visibility_Static(&FHeartGraphAssetEditor::GetDebuggerVisibility)
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
	return HeartGraph.IsValid() ? FText::FromName(HeartGraph->GetFName()) : FText(); // @todo GetFName could be replaced by GetDisplayName, etc
}

//////////////////////////////////////////////////////////////////////////
// Heart Graph Asset Toolbar

FHeartGraphAssetToolbar::FHeartGraphAssetToolbar(const TSharedPtr<FHeartGraphAssetEditor> InAssetEditor, UToolMenu* ToolbarMenu)
	: HeartGraphAssetEditor(InAssetEditor)
{
	BuildAssetToolbar(ToolbarMenu);
	BuildDebuggerToolbar(ToolbarMenu);
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
		auto&& HeartGraph = HeartGraphAssetEditor.Pin()->GetHeartGraph();
		if (HeartGraph)
		{
			FString Filename = SourceControlHelpers::PackageFilename(HeartGraph->GetPathName());
			TWeakObjectPtr<UObject> AssetPtr = HeartGraph;

			// Add our async SCC task widget
			// @TODO
			//return SNew(SAssetRevisionMenu, Filename)
			//	.OnRevisionSelected_Static(&OnDiffRevisionPicked, AssetPtr);
		}
		else
		{
			// if asset is null then this means that multiple assets are selected
			FMenuBuilder MenuBuilder(true, nullptr);
			MenuBuilder.AddMenuEntry(LOCTEXT("NoRevisionsForMultipleHeartGraphAssets", "Multiple Heart Graph Assets selected"), FText(), FSlateIcon(), FUIAction());
			return MenuBuilder.MakeWidget();
		}
	}

	FMenuBuilder MenuBuilder(true, nullptr);
	MenuBuilder.AddMenuEntry(LOCTEXT("SourceControlDisabled", "Source control is disabled"), FText(), FSlateIcon(), FUIAction());
	return MenuBuilder.MakeWidget();
}

void FHeartGraphAssetToolbar::BuildDebuggerToolbar(UToolMenu* ToolbarMenu)
{
	FToolMenuSection& Section = ToolbarMenu->AddSection("Debugging");
	Section.InsertPosition = FToolMenuInsert("Asset", EToolMenuInsertType::After);

	FPlayWorldCommands::BuildToolbar(Section);

	auto&& TemplateAsset = HeartGraphAssetEditor.Pin()->GetHeartGraph();

	Breadcrumb = SNew(SHeartGraphAssetBreadcrumb, TemplateAsset);
	Section.AddEntry(FToolMenuEntry::InitWidget("AssetBreadcrumb", Breadcrumb.ToSharedRef(), FText(), true));
}

#undef LOCTEXT_NAMESPACE
