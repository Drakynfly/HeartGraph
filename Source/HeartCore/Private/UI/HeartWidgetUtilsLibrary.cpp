// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetUtilsLibrary.h"
#include "UI/HeartInputActivation.h"
#include "UI/HeartWidgetInputLinker.h"

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
