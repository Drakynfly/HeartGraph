// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Preview/PreviewSceneViewportClient.h"
#include "Preview/SPreviewSceneViewport.h"
#include "Preview/HeartPreviewScene.h"

namespace Heart::AssetEditor
{
	FPreviewSceneViewportClient::FPreviewSceneViewportClient(const TSharedRef<SEditorViewport>& InThumbnailViewport,
															 const TSharedRef<FHeartPreviewScene>& InPreviewScene)
		: FEditorViewportClient(nullptr, &InPreviewScene.Get(), StaticCastSharedRef<SEditorViewport>(InThumbnailViewport))
		, ViewportPtr(InThumbnailViewport)
	{
		AdvancedPreviewScene = static_cast<FHeartPreviewScene*>(PreviewScene);

		SetRealtime(true);

		// Hide grid, we don't need this.
		DrawHelper.bDrawGrid = false;
		DrawHelper.bDrawPivot = false;
		DrawHelper.AxesLineThickness = 5;
		DrawHelper.PivotSize = 5;

		//Initiate view
		SetViewLocation(FVector(75, 75, 75));
		SetViewRotation(FVector(-75, -75, -75).Rotation());

		EngineShowFlags.SetScreenPercentage(true);

		// Set the Default type to Ortho and the XZ Plane
		ELevelViewportType NewViewportType = LVT_Perspective;
		FEditorViewportClient::SetViewportType(NewViewportType);

		// View Modes in Persp and Ortho
		SetViewModes(VMI_Lit, VMI_Lit);
	}

	void FPreviewSceneViewportClient::FocusViewportOnBounds(const FBoxSphereBounds Bounds, bool bInstant /*= false*/)
	{
		const FVector Position = Bounds.Origin;
		double Radius = Bounds.SphereRadius;

		float AspectToUse = AspectRatio;
		FIntPoint ViewportSize = Viewport->GetSizeXY();
		if (!bUseControllingActorViewInfo && ViewportSize.X > 0 && ViewportSize.Y > 0)
		{
			AspectToUse = Viewport->GetDesiredAspectRatio();
		}

		const bool bEnable = false;
		ToggleOrbitCamera(bEnable);

		/**
		* We need to make sure we are fitting the sphere into the viewport completely, so if the height of the viewport is less
		* than the width of the viewport, we scale the radius by the aspect ratio in order to compensate for the fact that we have
		* less visible vertically than horizontally.
		*/
		if (AspectToUse > 1.0)
		{
			Radius *= AspectToUse;
		}

		/**
		* Now that we have a adjusted radius, we are taking half of the viewport's FOV,
		* converting it to radians, and then figuring out the camera's distance from the center
		* of the bounding sphere using some simple trig.  Once we have the distance, we back up
		* along the camera's forward vector from the center of the sphere, and set our new view location.
		*/
		const float HalfFOVRadians = FMath::DegreesToRadians(ViewFOV / 2.0f);
		const double DistanceFromSphere = Radius / FMath::Sin(HalfFOVRadians);
		FViewportCameraTransform& ViewTransform = GetViewTransform();
		FVector CameraOffsetVector = ViewTransform.GetRotation().Vector() * -DistanceFromSphere;

		ViewTransform.SetLookAt(Position);
		ViewTransform.TransitionToLocation(Position + CameraOffsetVector, EditorViewportWidget, bInstant);

		// Tell the viewport to redraw itself.
		Invalidate();
	}
}