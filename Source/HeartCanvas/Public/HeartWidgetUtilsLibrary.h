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
	static FVector2D UINavigationToVector(EUINavigation Navigation);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static FVector2D GetGeometryCenter(const FGeometry& Geometry);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static void DrawSpline(UPARAM(ref) FPaintContext& Context, const FVector2D& From, const FVector2D& FromTangent,
		const FVector2D& To, const FVector2D& ToTangent, float Thickness, const FLinearColor& Tint);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static void DrawCubicBezierSpline(UPARAM(ref) FPaintContext& Context, const FVector2D& P0, const FVector2D& P1,
		const FVector2D& P2, const FVector2D& P3, float Thickness, const FLinearColor& Tint);

	/** Get the middle point of the widget in local space */
	UFUNCTION(BlueprintPure, Category = "Heart|WidgetUtilsLibrary")
	static FVector2D GetWidgetCenterLocal(const UWidget* Widget);

	/** Get the middle point of the widget in absolute space */
	UFUNCTION(BlueprintPure, Category = "Heart|WidgetUtilsLibrary")
	static FVector2D GetWidgetCenterAbsolute(const UWidget* Widget);

	//@todo really, these should be custom K2Nodes, so that they can have expose on spawn pins, but im too lazy to do that rn

	/** Creates a UUserWidget, using any widget as an outer. */
	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary", meta = (DeterminesOutputType = "Class"))
	static UUserWidget* CreateWidgetWithWidgetOuter(UWidget* Outer, TSubclassOf<UUserWidget> Class);

	/** Creates a UUserWidget, using the game instance as an outer. */
	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary", meta = (DeterminesOutputType = "Class"))
	static UUserWidget* CreateWidgetWithGameInstanceOuter(UGameInstance* Outer, TSubclassOf<UUserWidget> Class);
};