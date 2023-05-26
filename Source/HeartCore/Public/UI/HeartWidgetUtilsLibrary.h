// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartWidgetUtilsLibrary.generated.h"

class UHeartWidgetInputBindingAsset;

namespace Heart::Input
{
	// Stolen from UWidgetInteractionComponent
	static void GetKeyAndCharCodes(const FKey& Key, bool& bHasKeyCode, uint32& KeyCode, bool& bHasCharCode, uint32& CharCode)
	{
		const uint32* KeyCodePtr;
		const uint32* CharCodePtr;
		FInputKeyManager::Get().GetCodesFromKey(Key, KeyCodePtr, CharCodePtr);

		bHasKeyCode = KeyCodePtr ? true : false;
		bHasCharCode = CharCodePtr ? true : false;

		KeyCode = KeyCodePtr ? *KeyCodePtr : 0;
		CharCode = CharCodePtr ? *CharCodePtr : 0;

		// These special keys are not handled by the platform layer, and while not printable
		// have character mappings that several widgets look for, since the hardware sends them.
		if (CharCodePtr == nullptr)
		{
			if (Key == EKeys::Tab)
			{
				CharCode = '\t';
				bHasCharCode = true;
			}
			else if (Key == EKeys::BackSpace)
			{
				CharCode = '\b';
				bHasCharCode = true;
			}
			else if (Key == EKeys::Enter)
			{
				CharCode = '\n';
				bHasCharCode = true;
			}
		}
	}

	static FKeyEvent MakeKeyEventFromKey(const FKey& Key)
	{
		bool bHasKeyCode, bHasCharCode;
		uint32 KeyCode, CharCode;
		GetKeyAndCharCodes(Key, bHasKeyCode, KeyCode, bHasCharCode, CharCode);

		return FKeyEvent(Key, FModifierKeysState(), 0, false, KeyCode, CharCode);
	}
}

/**
 *
 */
UCLASS()
class HEARTCORE_API UHeartWidgetUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Heart|WidgetUtilsLibrary")
	static FVector2D UINavigationToVector(EUINavigation Navigation);

	UFUNCTION(BlueprintPure, Category = "Heart|WidgetUtilsLibrary")
	static int32 FindClosestToDirection(const TArray<FVector2D>& Locations, FVector2D From, FVector2D Direction, float DotRange = 0.5);

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


	// These might be suited better in another place. . .

	/**
	 * Get all manually-keyed actions for a widget. This only works if either the widget, or one of its parents,
	 * implements IHeartWidgetInputLinkerRedirector.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static TArray<struct FHeartManualInputQueryResult> GetActionsForWidget(const UWidget* Widget);

	/**
	 * Trigger a manually-keyed action for a widget. This only works if either the widget, or one of its parents,
	 * implements IHeartWidgetInputLinkerRedirector.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static bool TriggerManualInput(UWidget* Widget, FName Key);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static bool BindInputsToWidget(UWidget* Widget, UHeartWidgetInputBindingAsset* BindingAsset);

	UFUNCTION(BlueprintCallable, Category = "Heart|WidgetUtilsLibrary")
	static bool UnbindInputsFromWidget(UWidget* Widget, UHeartWidgetInputBindingAsset* BindingAsset);
};
