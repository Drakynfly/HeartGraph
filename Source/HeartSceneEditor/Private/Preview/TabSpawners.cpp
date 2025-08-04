// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "TabSpawners.h"
#include "PropertyEditorModule.h"
#include "Graph/HeartGraphAssetEditor.h"
#include "Modules/ModuleManager.h"
#include "Preview/ApplicationMode_PreviewScene.h"
#include "Preview/HeartPreviewScene.h"
#include "Preview/PreviewSceneConfig.h"
#include "Preview/SPreviewSceneViewport.h"

#define LOCTEXT_NAMESPACE "Heart::AssetEditor::TabSpawners"

namespace Heart::AssetEditor
{
	/*-------------------
		PREVIEW SCENE
	 -------------------*/

	const FLazyName FPreviewSceneSummoner::TabId("Heart_AssetEditor_PreviewScene");

	FPreviewSceneSummoner::FPreviewSceneSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor,
		const FOnPreviewSceneCreated& OnPreviewSceneCreated)
	  : FWorkflowTabFactory(TabId, AssetEditor)
	{
		TabLabel = LOCTEXT("PreviewSceneTabLabel", "Preview");
		TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Viewports");
		bIsSingleton = true;

		ViewMenuDescription = LOCTEXT("PreviewScene_Description", "");
		ViewMenuTooltip = LOCTEXT("PreviewScene_ToolTip", "");

		PreviewViewport = SNew(SPreviewSceneViewport, AssetEditor,
			MakeShared<FHeartPreviewScene>(
				FPreviewScene::ConstructionValues()
				.AllowAudioPlayback(true)
				.ShouldSimulatePhysics(true)
				.ForceUseMovementComponentInNonGameWorld(true),
				AssetEditor.ToSharedRef()));

		OnPreviewSceneCreated.Execute(PreviewViewport.ToSharedRef());
	}

	TSharedRef<SWidget> FPreviewSceneSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
	{
		PreviewViewport->Refresh();
		return PreviewViewport.ToSharedRef();
	}

	FText FPreviewSceneSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
	{
		return LOCTEXT("PreviewScene_TabTooltip", "");
	}


	/*-------------------
	PREVIEW SCENE DETAILS
	 -------------------*/

	const FLazyName FPreviewSceneDetailsPanelSummoner::TabId("Heart_AssetEditor_PreviewSceneDetails");

	FPreviewSceneDetailsPanelSummoner::FPreviewSceneDetailsPanelSummoner(
		TSharedPtr<FHeartGraphEditor> AssetEditor, const FOnDetailsCreated& OnDetailsCreated)
	  : FWorkflowTabFactory(TabId, AssetEditor), Callback(OnDetailsCreated)
	{
		TabLabel = LOCTEXT("PreviewSceneDetailsTabLabel", "Details");
		TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details");
		bIsSingleton = true;

		ViewMenuDescription = LOCTEXT("PreviewSceneDetails_Description", "");
		ViewMenuTooltip = LOCTEXT("PreviewSceneDetails_ToolTip", "");
	}

	TSharedRef<SWidget> FPreviewSceneDetailsPanelSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		FDetailsViewArgs Args;
		Args.bHideSelectionTip = true;
		Args.bShowPropertyMatrixButton = false;
		Args.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;

		auto DetailsView = PropertyModule.CreateDetailView(Args);

		Callback.Execute(DetailsView);

		return DetailsView;
	}

	FText FPreviewSceneDetailsPanelSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
	{
		return LOCTEXT("PreviewSceneDetailsPanel_TabTooltip", "");
	}
}

#undef LOCTEXT_NAMESPACE