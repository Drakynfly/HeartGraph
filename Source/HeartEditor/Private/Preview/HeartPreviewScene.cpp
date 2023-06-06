// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Preview/HeartPreviewScene.h"

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

		UStaticMesh* PreviewMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/EngineMeshes/Cube.Cube"), nullptr, LOAD_None, nullptr);
		FTransform PreviewMeshTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(1.0f, 1.0f, 1.0f ));

		{
			PreviewComponent = NewObject<UStaticMeshComponent>(GetTransientPackage());
			PreviewComponent->SetStaticMesh(PreviewMesh);
			PreviewComponent->bSelectable = true;

			FAdvancedPreviewScene::AddComponent(PreviewComponent, PreviewMeshTransform);
		}
	}

	FHeartPreviewScene::~FHeartPreviewScene()
	{

	}

	void FHeartPreviewScene::Tick(float InDeltaTime)
	{
		FAdvancedPreviewScene::Tick(InDeltaTime);
	}
}