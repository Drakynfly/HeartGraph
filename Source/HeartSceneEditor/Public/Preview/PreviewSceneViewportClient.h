// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "EditorViewportClient.h"
#include "SEditorViewport.h"

namespace Heart::AssetEditor
{
	class FHeartPreviewScene;

	/**
	 * FEditorViewportClient is generally responsible for handling the viewport, camera movement, and
	 * any of the options you'd find under the "Lit" menu on the standard Unreal Engine main viewport.
	 */
	class FPreviewSceneViewportClient : public FEditorViewportClient
	{
	public:
		/* Constructor and destructor */
		FPreviewSceneViewportClient(const TSharedRef<SEditorViewport>& InThumbnailViewport, const TSharedRef<FHeartPreviewScene>& InPreviewScene);

		/* Shameless stolen from SMaterialEditorViewport because for some reason a basic Focus on Selection is not implemented in the ViewportClient base class. */
		void FocusViewportOnBounds(const FBoxSphereBounds Bounds, bool bInstant /*= false*/);

		/** Pointer back to the Editor Viewport */
		TWeakPtr<SEditorViewport> ViewportPtr;

		/* Stored pointer to the preview scene in which the graph visualizer is shown */
		FHeartPreviewScene* AdvancedPreviewScene;
	};
}
