// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartWidgetUtilsLibrary.generated.h"

class UWidget;
struct FPaintContext;

/**
 *
 */
UCLASS()
class HEARTCANVAS_API UHeartWidgetUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Heart|WidgetUtilsLibrary")
	static FVector2f UINavigationToVector(EUINavigation Navigation);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static FVector2f GetGeometryCenter(const FGeometry& Geometry);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static void DrawSpline(UPARAM(ref) FPaintContext& Context, const FVector2f& From, const FVector2f& FromTangent,
		const FVector2f& To, const FVector2f& ToTangent, float Thickness, const FLinearColor& Tint);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static void DrawCubicBezierSpline(UPARAM(ref) FPaintContext& Context, const FVector2f& P0, const FVector2f& P1,
		const FVector2f& P2, const FVector2f& P3, float Thickness, const FLinearColor& Tint);

	/** Get the middle point of the widget in local space */
	UFUNCTION(BlueprintPure, Category = "Heart|WidgetUtilsLibrary")
	static FVector2f GetWidgetCenterLocal(const UWidget* Widget);

	/** Get the middle point of the widget in absolute space */
	UFUNCTION(BlueprintPure, Category = "Heart|WidgetUtilsLibrary")
	static FVector2f GetWidgetCenterAbsolute(const UWidget* Widget);

	/** Shortcut function to turn a boolean into a SlateVisibility */
	UFUNCTION(BlueprintPure, Category = "Heart|WidgetUtilsLibrary")
	static ESlateVisibility VisibleOrCollapsed(bool Value);

	/** Shortcut function to turn a boolean into a SlateVisibility */
	UFUNCTION(BlueprintPure, Category = "Heart|WidgetUtilsLibrary")
	static ESlateVisibility VisibleOrHidden(bool Value);

	//@todo really, these should be custom K2Nodes, so that they can have expose on spawn pins, but im too lazy to do that rn

	/** Creates a UUserWidget, using any widget as an outer. */
	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary", meta = (DefaultToSelf = "Outer", DeterminesOutputType = "Class"))
	static UUserWidget* CreateWidgetWithWidgetOuter(UWidget* Outer, TSubclassOf<UUserWidget> Class);

	/** Creates a UUserWidget, using the game instance as an outer. */
	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary", meta = (DeterminesOutputType = "Class"))
	static UUserWidget* CreateWidgetWithGameInstanceOuter(UGameInstance* Outer, TSubclassOf<UUserWidget> Class);

	/** Creates a UUserWidget, and assigns it to another widget as a tooltip */
	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary", meta = (DefaultToSelf = "Target", DeterminesOutputType = "Class"))
	static UUserWidget* CreateWidgetAsTooltip(UWidget* Target, TSubclassOf<UUserWidget> Class);
};