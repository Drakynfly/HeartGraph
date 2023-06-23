// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "TabSpawners.h"
#include "Graph/HeartGraphAssetEditor.h"

#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "Heart::AssetEditor::TabSpawners"

namespace Heart::AssetEditor
{
	/*-------------------
		PREVIEW SCENE
	 -------------------*/

	const FName FPreviewCanvasSummoner::TabId("Heart_AssetEditor_PreviewCanvas");

	FPreviewCanvasSummoner::FPreviewCanvasSummoner(TSharedPtr<FHeartGraphEditor> AssetEditor)
	  : FWorkflowTabFactory(TabId, AssetEditor)
	{
		TabLabel = LOCTEXT("PreviewCanvasTabLabel", "Preview");
		TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Viewports");
		bIsSingleton = true;

		ViewMenuDescription = LOCTEXT("PreviewCanvas_Description", "");
		ViewMenuTooltip = LOCTEXT("PreviewCanvas_ToolTip", "");
	}

	TSharedRef<SWidget> FPreviewCanvasSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
	{
		return SNullWidget::NullWidget;
	}

	FText FPreviewCanvasSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
	{
		return LOCTEXT("PreviewCanvas_TabTooltip", "");
	}


	/*-------------------
	PREVIEW SCENE DETAILS
	 -------------------*/

	const FName FPreviewCanvasDetailsPanelSummoner::TabId("Heart_AssetEditor_PreviewCanvasDetails");

	FPreviewCanvasDetailsPanelSummoner::FPreviewCanvasDetailsPanelSummoner(
		TSharedPtr<FHeartGraphEditor> AssetEditor, const FOnDetailsCreated& OnDetailsCreated)
	  : FWorkflowTabFactory(TabId, AssetEditor), Callback(OnDetailsCreated)
	{
		TabLabel = LOCTEXT("PreviewCanvasDetailsTabLabel", "Details");
		TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details");
		bIsSingleton = true;

		ViewMenuDescription = LOCTEXT("PreviewCanvasDetails_Description", "");
		ViewMenuTooltip = LOCTEXT("PreviewCanvasDetails_ToolTip", "");
	}

	TSharedRef<SWidget> FPreviewCanvasDetailsPanelSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
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

	FText FPreviewCanvasDetailsPanelSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
	{
		return LOCTEXT("PreviewCanvasDetailsPanel_TabTooltip", "");
	}
}

#undef LOCTEXT_NAMESPACE