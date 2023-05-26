// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetUtilsLibrary.h"
#include "UI/HeartInputActivation.h"
#include "UI/HeartWidgetInputBindingAsset.h"
#include "UI/HeartWidgetInputLinker.h"

namespace Heart::UI
{
	namespace Vectors
	{
		// UI directions
		static const FVector2D Left	( 1.0,  0.0);
		static const FVector2D Right(-1.0,  0.0);
		static const FVector2D Up	( 0.0,  1.0);
		static const FVector2D Down	( 0.0, -1.0);
	}
}

FVector2D UHeartWidgetUtilsLibrary::UINavigationToVector(const EUINavigation Navigation)
{
	switch (Navigation) {
	case EUINavigation::Left: return Heart::UI::Vectors::Left;
	case EUINavigation::Right: return Heart::UI::Vectors::Right;
	case EUINavigation::Up: return Heart::UI::Vectors::Up;
	case EUINavigation::Down: return Heart::UI::Vectors::Down;
	case EUINavigation::Next:
	case EUINavigation::Previous:
	case EUINavigation::Num:
	case EUINavigation::Invalid:
	default: return FVector2D::ZeroVector;
	}
}

int32 UHeartWidgetUtilsLibrary::FindClosestToDirection(const TArray<FVector2D>& Locations, const FVector2D From,
													   const FVector2D Direction, const float DotRange)
{
	int32 Closest = INDEX_NONE;
	double BestScore = 0;

	for (int32 i = 0; i < Locations.Num(); ++i)
	{
		FVector2D Offset = From - Locations[i];

		// Angle between direction and target
		const float Dot = Offset.GetSafeNormal() | Direction;

		if (Dot > DotRange)
		{
			const double Distance = FVector2D::Distance(From, Locations[i]);

			if (Distance > BestScore)
			{
				BestScore = Distance;
				Closest = i;
			}
		}
	}

	return Closest;
}

FVector2D UHeartWidgetUtilsLibrary::GetGeometryCenter(const FGeometry& Geometry)
{
	return Geometry.GetLocalSize() * 0.5;
}

void UHeartWidgetUtilsLibrary::DrawSpline(FPaintContext& Context, const FVector2D& From, const FVector2D& FromTangent,
                                          const FVector2D& To, const FVector2D& ToTangent, const float Thickness, const FLinearColor& Tint)
{
	Context.MaxLayer++;

	FSlateDrawElement::MakeSpline(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		From,
		FromTangent,
		To,
		ToTangent,
		Thickness,
		ESlateDrawEffect::None,
		Tint);
}

void UHeartWidgetUtilsLibrary::DrawCubicBezierSpline(FPaintContext& Context, const FVector2D& P0, const FVector2D& P1,
	const FVector2D& P2, const FVector2D& P3, const float Thickness, const FLinearColor& Tint)
{
	Context.MaxLayer++;

	FSlateDrawElement::MakeCubicBezierSpline(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		P0,	P1,	P2,	P3,
		Thickness,
		ESlateDrawEffect::None,
		Tint);
}

FVector2D UHeartWidgetUtilsLibrary::GetWidgetCenterLocal(const UWidget* Widget)
{
	if (!ensure(IsValid(Widget)))
	{
		return FVector2D::ZeroVector;
	}

	return GetGeometryCenter(Widget->GetTickSpaceGeometry());
}

FVector2D UHeartWidgetUtilsLibrary::GetWidgetCenterAbsolute(const UWidget* Widget)
{
	if (!ensure(IsValid(Widget)))
	{
		return FVector2D::ZeroVector;
	}

	auto&& Geometry = Widget->GetTickSpaceGeometry();
	return Geometry.LocalToAbsolute(GetGeometryCenter(Geometry));
}

UUserWidget* UHeartWidgetUtilsLibrary::CreateWidgetWithWidgetOuter(UWidget* Outer, const TSubclassOf<UUserWidget> Class)
{
	if (!ensure(IsValid(Outer)))
	{
		return nullptr;
	}

	if (!ensure(IsValid(Class)))
	{
		return nullptr;
	}

	return CreateWidget(Outer, Class);
}

UUserWidget* UHeartWidgetUtilsLibrary::CreateWidgetWithGameInstanceOuter(UGameInstance* Outer,
                                                                         const TSubclassOf<UUserWidget> Class)
{
	if (!ensure(IsValid(Outer)))
	{
		return nullptr;
	}

	if (!ensure(IsValid(Class)))
	{
		return nullptr;
	}

	return CreateWidget(Outer, Class);
}

TArray<FHeartManualInputQueryResult> UHeartWidgetUtilsLibrary::GetActionsForWidget(const UWidget* Widget)
{
	TArray<FHeartManualInputQueryResult> ActionList;

	auto&& Linker =  Heart::Input::FindLinkerForWidget(Widget);

	if (IsValid(Linker))
	{
		ActionList = Linker->QueryManualTriggers(Widget);
	}

	return ActionList;
}

bool UHeartWidgetUtilsLibrary::TriggerManualInput(UWidget* Widget, const FName Key)
{
	if (!IsValid(Widget) || Key.IsNone())
	{
		return false;
	}

	auto&& Linker =  Heart::Input::FindLinkerForWidget(Widget);

	if (IsValid(Linker))
	{
		return Linker->HandleManualInput(Widget, Key, FHeartInputActivation({1.f})).IsEventHandled();
	}

	return false;
}

bool UHeartWidgetUtilsLibrary::BindInputsToWidget(UWidget* Widget, UHeartWidgetInputBindingAsset* BindingAsset)
{
	if (!IsValid(Widget) || IsValid(BindingAsset))
	{
		return false;
	}

	auto&& Linker =  Heart::Input::FindLinkerForWidget(Widget);

	BindingAsset->BindLinker(Linker);
	return true;
}

bool UHeartWidgetUtilsLibrary::UnbindInputsFromWidget(UWidget* Widget, UHeartWidgetInputBindingAsset* BindingAsset)
{
	if (!IsValid(Widget) || IsValid(BindingAsset))
	{
		return false;
	}

	auto&& Linker =  Heart::Input::FindLinkerForWidget(Widget);

	BindingAsset->UnbindLinker(Linker);
	return true;
}
