// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Preview/SPreviewSceneViewport.h"

#include "Preview/PreviewSceneCommands.h"
#include "Preview/HeartPreviewScene.h"
#include "Preview/PreviewSceneViewportClient.h"

namespace Heart::AssetEditor
{
	void SPreviewSceneViewport::Construct(const FArguments& InArgs,
										 const TSharedPtr<FHeartGraphEditor>& InShowcaseAssetEditor,
										 const TSharedPtr<FHeartPreviewScene>& InPreviewScene)
	{
		EditorPtr = InShowcaseAssetEditor;
		PreviewScene = InPreviewScene;
		SEditorViewport::Construct(SEditorViewport::FArguments());
	}

	SPreviewSceneViewport::~SPreviewSceneViewport()
	{
		if (ViewportClient.IsValid())
		{
			ViewportClient->Viewport = nullptr;
			ViewportClient.Reset();
		}
	}

	TSharedRef<SEditorViewport> SPreviewSceneViewport::GetViewportWidget()
	{
		return SharedThis(this);
	}

	TSharedPtr<FExtender> SPreviewSceneViewport::GetExtenders() const
	{
		TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
		return Result;
	}

	void SPreviewSceneViewport::OnFloatingButtonClicked()
	{
		// Nothing
	}

	void SPreviewSceneViewport::OnFocusViewportToSelection()
	{
		SEditorViewport::OnFocusViewportToSelection();

		/* TODO: Replace with PreviewMeshComponent->Bounds */
		const FBoxSphereBounds Bounds = FBoxSphereBounds();
		ViewportClient->FocusViewportOnBounds( Bounds, false );
	}

	TSharedRef<FEditorViewportClient> SPreviewSceneViewport::MakeEditorViewportClient()
	{
		ViewportClient = MakeShareable(new FPreviewSceneViewportClient(SharedThis(this), PreviewScene.ToSharedRef()));
		return ViewportClient.ToSharedRef();
	}

	void SPreviewSceneViewport::BindCommands()
	{
		SEditorViewport::BindCommands();

		FPreviewSceneCommands::Register();
		const FPreviewSceneCommands& Commands = FPreviewSceneCommands::Get();

		CommandList->MapAction(Commands.FocusViewport,
			FExecuteAction::CreateSP(this, &SPreviewSceneViewport::FocusViewport));
	}

	void SPreviewSceneViewport::FocusViewport()
	{
		OnFocusViewportToSelection();
	}
}

