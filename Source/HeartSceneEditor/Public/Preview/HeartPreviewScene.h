// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "AdvancedPreviewScene.h"

class AHeartSceneActor;
class UPreviewSceneConfig;

namespace Heart::AssetEditor
{
	class FHeartGraphEditor;

	/**
	 *
	 */
	class FHeartPreviewScene : public FAdvancedPreviewScene
	{
	public:
		FHeartPreviewScene(const ConstructionValues& CVS, const TSharedRef<FHeartGraphEditor>& EditorToolkit);
		virtual ~FHeartPreviewScene() override;

		virtual void Tick(float InDeltaTime) override;

		TSharedRef<FHeartGraphEditor> GetEditor() const
		{
			return EditorPtr.Pin().ToSharedRef();
		}

		UPreviewSceneConfig* GetConfig() const { return SceneConfig; }

		void OnRefresh();

	protected:
		void OnConfigEdit(const FPropertyChangedEvent& PropertyChangedEvent);

		void ReconstructSceneActor();

		TObjectPtr<UPreviewSceneConfig> SceneConfig;
		TObjectPtr<AHeartSceneActor> SceneActor;

	private:
		TWeakPtr<FHeartGraphEditor> EditorPtr;
	};
}