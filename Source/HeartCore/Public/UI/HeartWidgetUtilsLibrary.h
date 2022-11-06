// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartWidgetUtilsLibrary.generated.h"

/**
 *
 */
UCLASS()
class HEARTCORE_API UHeartWidgetUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HeartWidgetUtilsLibrary")
	static void DrawSpline(UPARAM(ref) FPaintContext& Context, const FVector2D& From, const FVector2D& FromTangent,
		const FVector2D& To, const FVector2D& ToTangent, float Thickness, const FLinearColor& Tint);

	UFUNCTION(BlueprintCallable, Category = "HeartWidgetUtilsLibrary")
	static void DrawCubicBezierSpline(UPARAM(ref) FPaintContext& Context, const FVector2D& P0, const FVector2D& P1,
		const FVector2D& P2, const FVector2D& P3, float Thickness, const FLinearColor& Tint);

	/** Get the middle point of the widget in local space */
	UFUNCTION(BlueprintPure, Category = "HeartWidgetUtilsLibrary")
	static FVector2D GetWidgetCenterLocal(UWidget* Widget);

	/** Get the middle point of the widget in absolute space */
	UFUNCTION(BlueprintPure, Category = "HeartWidgetUtilsLibrary")
	static FVector2D GetWidgetCenterAbsolute(UWidget* Widget);
};
