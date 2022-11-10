// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartWidgetUtilsLibrary.generated.h"

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
};
