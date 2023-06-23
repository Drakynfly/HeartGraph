// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Preview/HeartPreviewScene.h"
#include "Preview/PreviewSceneConfig.h"

#include "HeartSceneActor.h"
#include "HeartSceneExtension.h"
#include "HeartSceneGenerator.h"

#include "Model/HeartGraph.h"
#include "Graph/HeartGraphAssetEditor.h"

#include "GameFramework/WorldSettings.h"

namespace Heart::AssetEditor
{
	FHeartPreviewScene::FHeartPreviewScene(ConstructionValues CVS, const TSharedRef<FHeartGraphEditor>& EditorToolkit)
		: FAdvancedPreviewScene(CVS)
		, EditorPtr(EditorToolkit)
	{
		// Disable killing actors outside of the world
		AWorldSettings* WorldSettings = GetWorld()->GetWorldSettings(true);
		WorldSettings->bEnableWorldBoundsChecks = false;

		//Hide default floor
		SetFloorVisibility(false, false);
	}

	FHeartPreviewScene::~FHeartPreviewScene()
	{
	}

	void FHeartPreviewScene::Tick(const float InDeltaTime)
	{
		FAdvancedPreviewScene::Tick(InDeltaTime);

		// Tick the scene world
		GetWorld()->Tick(LEVELTICK_All, InDeltaTime);
	}

	void FHeartPreviewScene::OnRefresh()
	{
		if (!IsValid(SceneConfig))
		{
			TSubclassOf<UPreviewSceneConfig> ConfigClass;

			if (auto&& SceneExtension = GetEditor()->GetHeartGraph()->GetExtension<UHeartSceneExtension>())
			{
				ConfigClass = SceneExtension->GetConfigClass();
			}

			if (!IsValid(ConfigClass))
			{
				ConfigClass = UPreviewSceneConfig::StaticClass();
			}

			// Add component that exposed settings for configuring the scene
			SceneConfig = NewObject<UPreviewSceneConfig>(GetTransientPackage(), ConfigClass);
			FAdvancedPreviewScene::AddComponent(SceneConfig, FTransform::Identity);

			SceneConfig->OnConfigEdit.BindRaw(this, &FHeartPreviewScene::OnConfigEdit);
		}

		ReconstructSceneActor();
	}

	void FHeartPreviewScene::OnConfigEdit(const FPropertyChangedEvent& PropertyChangedEvent)
	{
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UPreviewSceneConfig, SceneClassOverride))
		{
			ReconstructSceneActor();
		}
	}

	void FHeartPreviewScene::ReconstructSceneActor()
	{
		const UHeartGraph* Graph = GetEditor()->GetHeartGraph();

		UClass* SceneClass = nullptr;

		if (IsValid(SceneConfig) && IsValid(SceneConfig->SceneClassOverride))
		{
			SceneClass = SceneConfig->SceneClassOverride;
		}
		else if (auto&& SceneExtension = Graph->GetExtension<UHeartSceneExtension>())
		{
			SceneClass = SceneExtension->GetSceneClass();
		}

		if (SceneActor)
		{
			if (SceneActor->GetClass() != SceneClass)
			{
				SceneActor->Destroy();
				SceneActor = nullptr;
			}
		}

		if (!IsValid(SceneActor) && IsValid(SceneClass))
		{
			FActorSpawnParameters Params;
			SceneActor = GetWorld()->SpawnActor<AHeartSceneActor>(SceneClass);

			if (auto&& Generator = SceneActor->GetGenerator())
			{
				if (UHeartGraph* DupGraph = DuplicateObject<UHeartGraph>(Graph, SceneActor))
				{
					Generator->SetDisplayedGraph(DupGraph);
					{
						FEditorScriptExecutionGuard EditorScriptExecutionGuard;
						Generator->Regenerate();
					}
				}
			}
		}
	}
}
