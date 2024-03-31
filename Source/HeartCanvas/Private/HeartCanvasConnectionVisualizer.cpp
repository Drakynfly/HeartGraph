// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartCanvasConnectionVisualizer.h"
#include "HeartWidgetUtilsLibrary.h"

#include "Model/HeartGraphNode.h"
#include "UMG/HeartGraphCanvasNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasConnectionVisualizer)

// @todo move to blueprint function library
class FGeometryHelper_TEMPEMULATOR
{
public:
	static FVector2D VerticalMiddleLeftOf(const FGeometry& SomeGeometry)
	{
		const FVector2D GeometryDrawSize = SomeGeometry.GetDrawSize();
		return FVector2D(
			SomeGeometry.AbsolutePosition.X,
			SomeGeometry.AbsolutePosition.Y + GeometryDrawSize.Y/2 );
	}

	static FVector2D VerticalMiddleRightOf(const FGeometry& SomeGeometry)
	{
		const FVector2D GeometryDrawSize = SomeGeometry.GetDrawSize();
		return FVector2D(
			SomeGeometry.AbsolutePosition.X + GeometryDrawSize.X,
			SomeGeometry.AbsolutePosition.Y + GeometryDrawSize.Y/2 );
	}

	static FVector2D CenterOf(const FGeometry& SomeGeometry)
	{
		const FVector2D GeometryDrawSize = SomeGeometry.GetAbsoluteSize();
		return FVector2D(SomeGeometry.GetAbsolutePosition()) + (GeometryDrawSize * 0.5f);
	}

	static void ConvertToPoints(const FGeometry& Geom, TArray<FVector2D>& Points)
	{
		const FVector2D Size = Geom.GetDrawSize();
		const FVector2D Location = FVector2D(Geom.AbsolutePosition);

		int32 Index = Points.AddUninitialized(4);
		Points[Index++] = Location;
		Points[Index++] = Location + FVector2D(0.0f, Size.Y);
		Points[Index++] = Location + FVector2D(Size.X, Size.Y);
		Points[Index++] = Location + FVector2D(Size.X, 0.0f);
	}

	/** Find the point on line segment from LineStart to LineEnd which is closest to Point */
	static FVector2D FindClosestPointOnLine(const FVector2D& LineStart, const FVector2D& LineEnd, const FVector2D& TestPoint)
	{
		const FVector2D LineVector = LineEnd - LineStart;

		const float A = -FVector2D::DotProduct(LineStart - TestPoint, LineVector);
		const float B = LineVector.SizeSquared();
		const float T = FMath::Clamp<float>(A / B, 0.0f, 1.0f);

		// Generate closest point
		return LineStart + (T * LineVector);
	}

	static FVector2D FindClosestPointOnGeom(const FGeometry& Geom, const FVector2D& TestPoint)
	{
		TArray<FVector2D> Points;
		FGeometryHelper_TEMPEMULATOR::ConvertToPoints(Geom, Points);

		float BestDistanceSquared = MAX_FLT;
		FVector2D BestPoint;
		for (int32 i = 0; i < Points.Num(); ++i)
		{
			const FVector2D Candidate = FindClosestPointOnLine(Points[i], Points[(i + 1) % Points.Num()], TestPoint);
			const float CandidateDistanceSquared = (Candidate-TestPoint).SizeSquared();
			if (CandidateDistanceSquared < BestDistanceSquared)
			{
				BestPoint = Candidate;
				BestDistanceSquared = CandidateDistanceSquared;
			}
		}

		return BestPoint;
	}
};

void UHeartCanvasConnectionVisualizer::DrawConnectionSpline_Implementation(FPaintContext& Context,
	const FVector2D& Start, const FVector2D& End, const FHeartCanvasConnectionSplineParams& SplineParams) const
{
	UHeartWidgetUtilsLibrary::DrawSpline(Context, Start, {1.0, 0.0}, End, {1.0, 0.0}, SplineParams.Thickness, SplineParams.Color);
}

void UHeartCanvasConnectionVisualizer::PaintTimeDrawPinConnection_Implementation(FPaintContext& Context,
	const FVector2D& Start, const FVector2D& End, const FHeartCanvasConnectionPinParams& PinParams) const
{
	// @todo parameterize these
	FHeartCanvasConnectionSplineParams SplineParams;
	SplineParams.Color = FLinearColor::White;
	SplineParams.Thickness = 1.f;

	SplineParams.FromDirection = EnumHasAnyFlags(PinParams.FromPin->GetPinDesc().Direction, EHeartPinDirection::Input) ? EHeartPinDirection::Input : EHeartPinDirection::Output;
	SplineParams.ToDirection = EnumHasAnyFlags(PinParams.ToPin->GetPinDesc().Direction, EHeartPinDirection::Input) ? EHeartPinDirection::Input : EHeartPinDirection::Output;
	DrawConnectionSpline(Context, Start, End, SplineParams);
}

void UHeartCanvasConnectionVisualizer::PaintTimeDrawPreviewConnection_Implementation(FPaintContext& Context,
	const FVector2D& Start, const FVector2D& End, UHeartGraphCanvasPin* FromPin) const
{
	// @todo parameterize these
	FHeartCanvasConnectionSplineParams SplineParams;
	SplineParams.Color = FLinearColor::White;
	SplineParams.Thickness = 1.f;

	SplineParams.FromDirection = EnumHasAnyFlags(FromPin->GetPinDesc().Direction, EHeartPinDirection::Input) ? EHeartPinDirection::Input : EHeartPinDirection::Output;
	SplineParams.ToDirection = SplineParams.FromDirection == EHeartPinDirection::Input ? EHeartPinDirection::Output : EHeartPinDirection::Input;

	DrawConnectionSpline(Context, Start, End, SplineParams);
}

void UHeartCanvasConnectionVisualizer::PaintTimeDrawPinConnections(FPaintContext& Context, const FGeometry& GraphDesktopGeometry, TMap<FHeartPinGuid, TPair<UHeartGraphCanvasPin*, FGeometry>> Pins)
{
	for (TTuple<FHeartPinGuid, TTuple<UHeartGraphCanvasPin*, FGeometry>>& PinPair : Pins)
	{
		if (!ensureMsgf(PinPair.Key.IsValid(),
			TEXT("PaintTimeDrawPinConnections was given invalid UHeartGraphPin!")))
		{
			continue;
		}

		if (!ensureMsgf(IsValid(PinPair.Value.Key),
			TEXT("PaintTimeDrawPinConnections was given invalid UHeartGraphCanvasPin!")))
		{
			continue;
		}

		// Only draw connections from output pins to their connected inputs.
		if (!EnumHasAnyFlags(PinPair.Value.Key->GetPinDesc().Direction, EHeartPinDirection::Output))
		{
			continue;
		}

		auto&& StartGeom = PinPair.Value.Value;

		// @todo kinda awful that this is the only way ive found to do this...
		FVector2D StartPoint;

		FVector CustomPosition;
		bool RelativeStart;
		const bool HandledStart = IGraphPinVisualizerInterface::Execute_GetCustomAttachmentPosition(PinPair.Value.Key, CustomPosition, RelativeStart);

		if (!HandledStart || RelativeStart)
		{
			StartPoint = GraphDesktopGeometry.AbsoluteToLocal(StartGeom.LocalToAbsolute(UHeartWidgetUtilsLibrary::GetGeometryCenter(StartGeom)));
		}

		if (HandledStart)
		{
			StartPoint += FVector2D(CustomPosition);
		}

		TSet<FHeartGraphPinReference>& ConnectedPins = PinPair.Value.Key->GetCanvasNode()->GetGraphNode()->
		                                                       GetLinks(PinPair.Key).Links;
		for (FHeartGraphPinReference& ConnectedPin : ConnectedPins)
		{
			auto&& ConnectedPinAndGeo = Pins.Find(ConnectedPin.PinGuid);
			if (!ConnectedPinAndGeo) continue;

			auto&& EndGeom = ConnectedPinAndGeo->Value;

			FVector2D EndPoint;
			bool RelativeEnd;
			const bool HandledEnd = IGraphPinVisualizerInterface::Execute_GetCustomAttachmentPosition(ConnectedPinAndGeo->Key, CustomPosition, RelativeEnd);

			if (!HandledEnd || RelativeEnd)
			{
				// @todo kinda awful that this is the only way ive found to do this...
				EndPoint = GraphDesktopGeometry.AbsoluteToLocal(EndGeom.LocalToAbsolute(UHeartWidgetUtilsLibrary::GetGeometryCenter(EndGeom)));
			}

			if (HandledEnd)
			{
				EndPoint += FVector2D(CustomPosition);
			}

			//const FVector2D StartPoint = FGeometryHelper_TEMPEMULATOR::CenterOf(StartGeom);
			//const FVector2D EndPoint = FGeometryHelper_TEMPEMULATOR::CenterOf(EndGeom);

			PaintTimeDrawPinConnection(Context, StartPoint, EndPoint, { PinPair.Value.Key, ConnectedPinAndGeo->Key });
		}
	}
}