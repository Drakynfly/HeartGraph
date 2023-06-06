// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "AdvancedPreviewScene.h"

namespace Heart::AssetEditor
{
	class FHeartGraphEditor;

	/**
	 *
	 */
	class FHeartPreviewScene : public FAdvancedPreviewScene
	{
	public:
		FHeartPreviewScene(ConstructionValues CVS, const TSharedRef<FHeartGraphEditor>& EditorToolkit);
		virtual ~FHeartPreviewScene() override;

		virtual void Tick(float InDeltaTime) override;

		TSharedRef<FHeartGraphEditor> GetEditor() const
		{
			return EditorPtr.Pin().ToSharedRef();
		}

		UStaticMeshComponent* PreviewComponent = nullptr;

	private:
		TWeakPtr<FHeartGraphEditor> EditorPtr;
	};
}