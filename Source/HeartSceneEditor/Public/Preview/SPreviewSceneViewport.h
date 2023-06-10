// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "SCommonEditorViewportToolbarBase.h"
#include "SEditorViewport.h"

namespace Heart::AssetEditor
{
	class FHeartGraphEditor;
	class FHeartPreviewScene;
	class FPreviewSceneViewportClient;

	/**
	 *
	 */
	class SPreviewSceneViewport : public SEditorViewport, public FGCObject, public ICommonEditorViewportToolbarInfoProvider
	{
	public:
		SLATE_BEGIN_ARGS(SPreviewSceneViewport) {}
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, const TSharedPtr<FHeartGraphEditor>& InShowcaseAssetEditor, const TSharedPtr<FHeartPreviewScene>& InPreviewScene);
		virtual ~SPreviewSceneViewport() override;

		//~ FGCObject
		virtual void AddReferencedObjects(FReferenceCollector& Collector) override {}
		//~

		//~ ICommonEditorViewportToolbarInfoProvider
		virtual TSharedRef<SEditorViewport> GetViewportWidget() override;
		virtual TSharedPtr<FExtender> GetExtenders() const override;
		virtual void OnFloatingButtonClicked() override;
		//~

		//~ SEditorViewport
		virtual void OnFocusViewportToSelection() override;
		virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
		//~

		virtual void BindCommands() override;

		TSharedPtr<FHeartPreviewScene> GetPreviewScene() { return PreviewScene; }
		TSharedPtr<FPreviewSceneViewportClient> GetViewportClient() { return ViewportClient; }

		void Refresh();

		void FocusViewport();

	protected:
		/** The scene for this viewport. */
		TSharedPtr<FHeartPreviewScene> PreviewScene;

		/** Shared ptr to the client */
		TSharedPtr<FPreviewSceneViewportClient> ViewportClient;

		/** Toolkit Pointer */
		TWeakPtr<FHeartGraphEditor> EditorPtr;
	};
}
