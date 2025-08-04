// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "HeartWidgetUtilsLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Engine/GameInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartWidgetUtilsLibrary)

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
	switch (Navigation)
	{
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

ESlateVisibility UHeartWidgetUtilsLibrary::VisibleOrCollapsed(const bool Value)
{
	return Value ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}

ESlateVisibility UHeartWidgetUtilsLibrary::VisibleOrHidden(const bool Value)
{
	return Value ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
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

UUserWidget* UHeartWidgetUtilsLibrary::CreateWidgetAsTooltip(UWidget* Target, const TSubclassOf<UUserWidget> Class)
{
	UUserWidget* Tooltip = CreateWidget(Target, Class);
	Target->SetToolTip(Tooltip);
	return Tooltip;
}