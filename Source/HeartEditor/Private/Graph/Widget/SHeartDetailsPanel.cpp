// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Graph/Widgets/SHeartDetailsPanel.h"

#include "Model/HeartGraph.h"

#include "SlateOptMacros.h"
#include "Graph/HeartGraphAssetEditor.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace Heart::AssetEditor
{
	void SDetailsPanel::Construct(const FArguments& InArgs, const TSharedPtr<FHeartGraphEditor>& AssetEditor)
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		FDetailsViewArgs Args;
		Args.bHideSelectionTip = true;
		Args.bShowPropertyMatrixButton = false;
		Args.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
		Args.NotifyHook = AssetEditor.Get();

		FIsPropertyEditingEnabled CanEditDelegate;
		CanEditDelegate.BindSP(AssetEditor.ToSharedRef(), &FHeartGraphEditor::CanEdit);

		DetailsView_Graph = PropertyModule.CreateDetailView(Args);
		DetailsView_Graph->SetIsPropertyEditingEnabledDelegate(CanEditDelegate);
		DetailsView_Graph->SetObject(AssetEditor->GetHeartGraph());

		DetailsView_Object = PropertyModule.CreateDetailView(Args);
		DetailsView_Object->SetIsPropertyEditingEnabledDelegate(CanEditDelegate);
		DetailsView_Object->SetObject(nullptr);

		ChildSlot
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
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
